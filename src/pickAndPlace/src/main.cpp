// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
//
// Author: Ugo Pattacini - <ugo.pattacini@iit.it>

#include <string>
#include <cmath>
#include <algorithm>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>

#include "helpers.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;


/***************************************************/
class CtrlModule: public RFModule
{
protected:
    PolyDriver drvArmR, drvArmL, drvGaze;
    PolyDriver drvHandR, drvHandL;
    ICartesianControl *iarm;
    IGazeControl      *igaze;
    int startup_ctxt_arm_right;
    int startup_ctxt_arm_left;
    int startup_ctxt_gaze;
    double initial_approach_height;

    RpcServer rpcPort;
    ObjectRetriever object;



    /***************************************************/
    Vector computeHandOrientation(const double  &hand_orientation)
    {
        // we have to provide a 4x1 vector representing the
        // final orientation for the specified hand, with
        // the palm pointing downward

        // roation for right hand

            int dir = 1;

            Matrix Rot(3,3);
            Rot(0,0)=-1.0;  Rot(0,1)= 0.0;  Rot(0,2)=0.0;
            Rot(1,0)=0.0;   Rot(1,1)= dir;  Rot(1,2)=0.0;
            Rot(2,0)=0.0;   Rot(2,1)=-0.0;  Rot(2,2)=-dir;

            Vector o(4);
            o[0] = 0.0;
            o[1] =-1.0;
            o[2] = 0.0;
            o[3] = hand_orientation * (M_PI / 180.0);

            Matrix RotY = axis2dcm(o).submatrix(0, 2, 0, 2);

            return dcm2axis(RotY * Rot);
        }


    /***************************************************/
    void approachTargetWithHand(const Vector &x,
                                const Vector &o)
    {

            drvArmR.view(iarm);


        // enable all dofs but the roll of the torso

        Vector dof(10, 1.0);
        dof[1] = 0.0; // roll is dof[1]
        Vector curDof;
        iarm->setDOF(dof, curDof);

        // reach the first via-point
        // located 5 cm above the target x

        Vector approach = x;
        approach[2] += initial_approach_height; // in m
        iarm->goToPoseSync(approach, o);
        iarm->waitMotionDone();

        // reach the final target x;

        iarm->goToPoseSync(x, o); // should use force feedback at some point
        iarm->waitMotionDone();
    }

    /***************************************************/
    void liftObject()
    {
        // select the correct interface

            drvArmR.view(iarm);

        // just lift the hand a few centimeters
        // wrt the current position

        Vector x, o;
        if (iarm->getPose(x, o))
        {
            x[2] += initial_approach_height; // lift 15cm, orientation stays the same
            iarm->goToPoseSync(x, o);
            iarm->waitMotionDone();
        }
    }

    /***************************************************/
    void moveFingers(
                     const VectorOf<int> &joints,
                     const double fingers_closure)
    {
        // select the correct interface
        IControlLimits   *ilim;
        IPositionControl *ipos;
        IControlMode2    *imod;

            drvHandR.view(ilim);
            drvHandR.view(ipos);
            drvHandR.view(imod);


        // enforce [0,1] interval
        double fingers_closure_sat=std::min(1.0,std::max(0.0,fingers_closure));

        // move each finger first:
        // if min_j and max_j are the minimum and maximum bounds of joint j,
        // then we should move to min_j+fingers_closure_sat*(max_j-min_j)

        for (size_t i=0; i<joints.size(); i++)
        {
            int j=joints[i];

            double min_j, max_j;
            if (ilim->getLimits(j, &min_j, &max_j))
            {
                double jointValue = min_j + fingers_closure_sat * (max_j - min_j); // formula that was given...
                ipos->setRefAcceleration(j, 200);
                ipos->setRefSpeed(j, 30);
                imod->setControlMode(j, VOCAB_CM_POSITION);
                ipos->positionMove(j, jointValue);
            }
        }

        // wait until all fingers have attained their set-points

            for (size_t i=0; i<joints.size(); i++)
            {
                bool jointDone = false;
                while(!jointDone){
                    ipos->checkMotionDone(joints[i], &jointDone);
                }
            }
            yInfo()<<"All finger joints have arrived";
    }

    /***************************************************/
    void home(const string &hand)
    {
        Vector home_x(3);
        home_x[0]=-0.2;
        home_x[2]=0.08;

        // select the correct interface
        if (hand=="right")
        {
            drvArmR.view(iarm);
            home_x[1]=0.3;
        }
        else
        {
            drvArmL.view(iarm);
            home_x[1]=-0.3;
        }

        igaze->lookAtAbsAngles(Vector(3,0.0));
        iarm->goToPositionSync(home_x);

        iarm->waitMotionDone();
        igaze->waitMotionDone();
        igaze->setTrackingMode(false);
    }

    /***************************************************/
    void look_down()
    {
        // we ask the controller to keep the vergence
        // from now on fixed at 5.0 deg, which is the
        // configuration where we calibrated the stereo-vision;
        // without that, we cannot retrieve good 3D positions
        // with the real robot
        igaze->blockEyes(5.0);
        Vector ang(3,0.0);
        ang[1]=-60.0;
        igaze->lookAtAbsAngles(ang);
        igaze->waitMotionDone();
    }

    /***************************************************/
    bool pick_or_place(const double grasp_configuration, const double hand_orientation, const Vector target_position, bool pick)
    {


        yInfo()<<"Starting the grasping";

        // refine the localization of the object
        // with a proper hand-related map

        yInfo()<<"refined 3D location = ("<<target_position.toString(3,3)<<")";

        Vector o = computeHandOrientation(hand_orientation);
        yInfo()<<"computed orientation = ("<<o.toString(3,3)<<")";

        // we set up here the lists of joints we need to actuate
        VectorOf<int> abduction,thumb,fingers;
        abduction.push_back(7);
        thumb.push_back(8);
        for (int i=9; i<16; i++)
            fingers.push_back(i);

        if(pick)
        {
            // let's put the hand in the pre-grasp configuration
            moveFingers(abduction,0.7);
            moveFingers(thumb,1.0);
            moveFingers(fingers,0.0);
            yInfo()<<"prepared hand";
        }
        else{
            // just keep the hand closed
        }

        approachTargetWithHand(target_position,o);
        yInfo()<<"approached object";

        if (pick){
            // grasp

            moveFingers(fingers,grasp_configuration);
            yInfo()<<"grasped";


            // picking ends here
        }
        else{
            // drop
            double open_offset = 0.2; // might need tuning
            moveFingers(fingers,grasp_configuration - open_offset); // open the hand
        }

        liftObject();
        yInfo()<<"lifted";

        return true;

    }

    /***************************************************/
    bool openCartesian(const string &robot, const string &arm)
    {
        PolyDriver &drvArm=(arm=="right_arm"?drvArmR:drvArmL);

        Property optArm;
        optArm.put("device","cartesiancontrollerclient");
        optArm.put("remote","/"+robot+"/cartesianController/"+arm);
        optArm.put("local","/cartesian_client/"+arm);

        // let's give the controller some time to warm up
        bool ok=false;
        double t0=Time::now();
        while (Time::now()-t0<10.0)
        {
            // this might fail if controller
            // is not connected to solver yet
            if (drvArm.open(optArm))
            {
                ok=true;
                break;
            }

            Time::delay(1.0);
        }

        if (!ok)
        {
            yError()<<"Unable to open the Cartesian Controller for "<<arm;
            return false;
        }
        return true;
    }

    bool openHandControl(std::string robot, PolyDriver& driver, std::string const& name)
    {
        Property optJoint;
        optJoint.put("device","remote_controlboard");
        optJoint.put("remote", "/"+ robot +"/"+ name);
        optJoint.put("local","/position/" + name);

        return driver.open(optJoint);
    }

public:
    /***************************************************/
    bool configure(ResourceFinder &rf)
    {
        string robot=rf.check("robot",Value("icubSim")).asString();

        initial_approach_height = 0.25;

        // we need both arms for this

        if (!openCartesian(robot,"right_arm"))
            return false;

        if (!openCartesian(robot,"left_arm"))
        {
            drvArmR.close();
            return false;
        }

        // initilize the gaze driver
        Property optGaze;
        optGaze.put("device","gazecontrollerclient");
        optGaze.put("remote","/iKinGazeCtrl");
        optGaze.put("local","/gaze_client");

        if (!drvGaze.open(optGaze))
        {
            yError() << "Unable to open Gaze controller";
            drvArmR.close();
            drvArmL.close();
            return false;
        }

        // and both hands

        if (!openHandControl(robot, drvHandL, "left_arm"))
        {
            yError()<<"Unable to connect to /icubSim/left_arm";
            return false;
        }

        if (!openHandControl(robot, drvHandR, "right_arm"))
        {
            yError()<<"Unable to connect to /icubSim/right_arm";
            return false;
        }

        // save startup contexts
        drvArmR.view(iarm);
        iarm->storeContext(&startup_ctxt_arm_right);

        drvArmL.view(iarm);
        iarm->storeContext(&startup_ctxt_arm_left);

        drvGaze.view(igaze);
        igaze->storeContext(&startup_ctxt_gaze);

        rpcPort.open("/service");
        attach(rpcPort);
        return true;
    }

    /***************************************************/
    bool interruptModule()
    {
        return true;
    }

    /***************************************************/
    bool close()
    {
        drvArmR.view(iarm);
        iarm->restoreContext(startup_ctxt_arm_right);

        drvArmL.view(iarm);
        iarm->restoreContext(startup_ctxt_arm_left);

        igaze->restoreContext(startup_ctxt_gaze);

        drvArmR.close();
        drvArmL.close();
        drvGaze.close();
        drvHandR.close();
        drvHandL.close();
        rpcPort.close();
        return true;
    }

    /***************************************************/
    bool respond(const Bottle &command, Bottle &reply)
    {
        string cmd=command.get(0).asString();
        if (cmd=="help")
        {
            reply.addVocab(Vocab::encode("many"));
            reply.addString("Available commands:");
            reply.addString("- look_down");
            reply.addString("- grasp_it");
            reply.addString("- quit");
        }
        else if (cmd=="look_down")
        {
            look_down();
            // we assume the robot is not moving now
            reply.addString("ack");
            reply.addString("Yep! I'm looking down now!");
        }
        else if (cmd=="grasp_it")
        {
            // the "closure" accounts for how much we should
            // close the fingers around the object:
            // if closure == 0.0, the finger joints have to reach their minimum
            // if closure == 1.0, the finger joints have to reach their maximum
            double fingers_closure = 0.5; // default value
            double hand_orientation = 30.0;

            // we can pass a new value via rpc
            if (command.size()>1)
                fingers_closure=command.get(1).asDouble();

            // we can pass a new value via rpc
            if (command.size()>2)
                hand_orientation=command.get(2).asDouble();

            Vector pick_location(3);
            pick_location[0] = -0.3;
            pick_location[1] = 0.0;
            pick_location[2] = 0.0;
            Vector place_location(3);
            place_location[0] = -0.3;
            place_location[1] = 0.2;
            place_location[2] = 0.0;

            pick_or_place(fingers_closure, hand_orientation, pick_location, true);
            bool ok = pick_or_place(fingers_closure, hand_orientation, place_location, false);
            // we assume the robot is not moving now
            if (ok)
            {
                reply.addString("ack");
                reply.addString("Yeah! I did it! Maybe...");
            }
            else
            {
                reply.addString("nack");
                reply.addString("I don't see any object!");
            }
        }
        else
            // the father class already handles the "quit" command
            return RFModule::respond(command,reply);
        return true;
    }

    /***************************************************/
    double getPeriod()
    {
        return 1.0;
    }

    /***************************************************/
    bool updateModule()
    {
        return true;
    }
};


/***************************************************/
int main(int argc, char *argv[])
{
    Network yarp;
    if (!yarp.checkNetwork())
    {
        yError()<<"YARP doesn't seem to be available";
        return 1;
    }

    CtrlModule mod;
    ResourceFinder rf;
    rf.configure(argc,argv);
    return mod.runModule(rf);
}

