/****************************************************************** 
**  Copyright: (C) VVV17, Santa Margherita
**  Copyright: (C) Team Orange
**  Authors: Pedro Vicente <pvicente@isr.tecnico.ulisboa.pt>
**  CopyPolicy: Released under the terms of the GNU GPL v2.0.
*******************************************************************/

#include "master.h"

using namespace yarp::os;
using namespace std;

double MasterModule::getPeriod() {
    return 0.0;
}

bool MasterModule::configure(yarp::os::ResourceFinder &rf) {


    //Closing/starting FALSE
    closing=false;
    starting=false;
    // Module name
    moduleName = rf.check("name", Value("master"),"Module name (string)").asString();
    setName(moduleName.c_str());

    string nameStarts  = rf.check("whostarts", Value("robot"),"Who starts the game? (human/robot) (string)").asString();
    Bottle *initBoard  = rf.find("initBoardPosition").asList();
    Bottle *GoalInit   = rf.find("GoalInit").asList();
    if(initBoard==NULL) {
        yError() << "unable to parse initBoardPosition";
        return false;
    }
    bool whostarts;
    // parsing information
    if(nameStarts == "human" ||nameStarts == "Human" || nameStarts == "0" || nameStarts == "HUMAN")
        whostarts = false; // human turn
    else
        whostarts = true; // robot turn
    setName(moduleName.c_str());
    rpcPortName = "/" + moduleName + "/rpc:i";


    // open RPC server port
    if (!rpcPort.open(rpcPortName.c_str()))
    {
        yError("%s : Unable to open port %s\n", getName().c_str(), rpcPortName.c_str());
        return false;
    }
    attach(rpcPort);

    threadPeriod = rf.check("threadPeriod", Value(0.033), "Master thread period key value(double) in seconds ").asDouble();

    // launch thread    
    comunThread = new MasterThread(moduleName,whostarts,threadPeriod);
    
    /* Start masterthread */
    if (!comunThread->start()) {
        delete comunThread;
        return false;
    }
    comunThread->mutexThread.lock();
    comunThread->BoardPose.resize(6);
    comunThread->BoardPose(0) = initBoard->get(0).asDouble();
    comunThread->BoardPose(1) = initBoard->get(1).asDouble();
    comunThread->BoardPose(2) = initBoard->get(2).asDouble();
    
    comunThread->BoardPose(3) = initBoard->get(3).asDouble();
    comunThread->BoardPose(4) = initBoard->get(4).asDouble();
    comunThread->BoardPose(5) = initBoard->get(5).asDouble();

    comunThread->startPose.resize(3);
    comunThread->startPose(0) = GoalInit->get(0).asDouble();
    comunThread->startPose(1) = GoalInit->get(1).asDouble();
    comunThread->startPose(2) = GoalInit->get(2).asDouble();
    yInfo() << "Board Pose: " << comunThread->BoardPose.toString();
    yInfo() << "Goal Pose: " << comunThread->startPose.toString();
    comunThread->mutexThread.unlock();
    // Everything ok.
    yInfo() << "MasterModule: " << "Everything ok";
    return true;

}

/*IDL Functions*/
bool MasterModule::quit() {
    yInfo() << "MasterModule: " << "Received Quit command in RPC";
    closing = true;
    return true;
}

// To update the model externally... just if needed
bool MasterModule::update(){
    return true;
}
bool MasterModule::start(){
    starting = true;
    yInfo() << "MasterModule: " << "Received Starting comming... Let's PLAY!!!";
    return true;
}
bool MasterModule::attach(RpcServer &source)
{
    return this->yarp().attachAsServer(source);

}

bool MasterModule::close() {
    yInfo() << "MasterModule: " << "Calling close function";
    rpcPort.close();
    comunThread->close();
    comunThread->stop();
    delete comunThread;
    return true;
}

bool MasterModule::interruptModule() {
    yInfo() <<"MasterModule: " << "Calling interrupt Module function";
    rpcPort.interrupt();
    comunThread->interrupt();
    return true;
}
bool MasterModule::updateModule() {
    // If nothing is connected to the master
    //yInfo() << "running";

    return !closing;
}

// reset the game
bool MasterModule::reset() {
    yInfo() << "MasterModule: " << "Received reset command in RPC";
    return true;
}
bool MasterModule::triggerNextMove() {
    yInfo() << "MasterModule: " << "Received triggerNextMove command in RPC";
    comunThread->mutexThread.lock();
    comunThread->clapReceived = true;
    comunThread->mutexThread.unlock();
    
    return true;
}


//*************************************************************************************************************************************//
//************                                                   MASTER THREAD                                                 ********//
//*************************************************************************************************************************************//
 
MasterThread::MasterThread(string Name, bool whostarts, int r) : RateThread(r) {
    moduleName = Name;
    myturn = whostarts;
};

void MasterThread::run(){
    // Game State machine...
    if(running) {
        if(rpcObjReco.getOutputCount()<=0 || rpcClap.getOutputCount()<=0 || rpcPlanner.getOutputCount()<=0 || rpcPickPlace.getOutputCount()<=0 || rpcGameState.getOutputCount()<=0) {
            yInfo() << "MasterThread: Waiting for connections...";        
            Time::delay(0.4);  
            return;      
        }
        else if(sendCommands){
                Bottle cmd,reply;
                cmd.clear();
                reply.clear();
                cmd.addString("setStart"); // Cups position NOT FINISHED!!!!!
                cmd.addDouble(0);
                cmd.addDouble(1);
                cmd.addDouble(2);
                rpcPickPlace.write(cmd,reply);
               if(reply.size () <= 0) {
                    yError() << "I have an empty Bottle from PicKPlace GoalPos...=/";
                    return;
                }
                else if(reply.get(0).asString()=="nack") {
                    yError() << "PicKPlace Set GoalPos not Ok";
                    return;
                }
                cmd.clear();
                reply.clear();
                mutexThread.lock();
                cmd.addString("init");
                cmd.addDouble(BoardPose[0]);
                cmd.addDouble(BoardPose[1]);
                cmd.addDouble(BoardPose[2]);
                cmd.addDouble(BoardPose[3]);
                cmd.addDouble(BoardPose[4]);
                cmd.addDouble(BoardPose[5]);
                mutexThread.unlock();
                rpcGameState.write(cmd,reply);
                if(reply.size () <= 0) {
                    yError() << "I have an empty Bottle from Game State BoardPos...=/";
                    return;
                }
                // Send to     
                sendCommands = false;    
            }
        stateMachine();
    }
}

void MasterThread::stateMachine() {

    Bottle cmd, reply;
    // Not My Turn
    if(!myturn) {
        yInfo() << "It's not my turn";
        mutexThread.lock();
        if(clapReceived) { 
            yInfo() << "But, I have received a Clap";
            myturn=true; 
            clapReceived = false;
            mutexThread.unlock();
            return;
        }
        else {
            mutexThread.unlock();
            Time::delay(0.5);
            return;
        }
    }
    // My Turn
    else {
        switch (statemyturn) {

            case 0: // GET 3D position of The Objects
            {
                yInfo() << "stateMyturn= " << statemyturn;
                yInfo() << "Sending Start Request to Object Recognition";
                cmd.clear();

                /*********/ 
                cmd.addString("start");
                rpcObjReco.write(cmd, reply);
                if(reply.size () <= 0) {
                    yError() << "I received a empty Bottle from the Object Recognition";
                    break;
                }
                // READ FROM STREAMING
                
                cmd.clear();
                cmd.addString("stop");
                rpcObjReco.write(cmd, reply);

                mutexThread.lock();
                ObjectLoc = reply;
                mutexThread.unlock();

                yInfo() << "Receive Object Recognition Bottle: ";
                yInfo() << reply.toString();
                statemyturn++;
            }
            case 1: // SEND OBJECT LOCATION TO GROUNDING AND RECEIVE NEXT 3D POSITION TO PUT THE OBJECT
            {
                yInfo() << "stateMyturn= " << statemyturn;
                if(reply.size () <= 0) {
                    yError() << "I have an empty Bottle stateturn is wrong...=/";
                    statemyturn=0;
                    yInfo() << "New State turn: " << statemyturn;
                    break;
                }
                yInfo() << "Sending Object Location to grounding aka gameState";
                cmd=reply;
                reply.clear();
                rpcGameState.write(cmd, reply);
                if(reply.size () <= 0) {
                    yError() << "I received a empty Bottle from the Grounding aka gameState";
                    break;
                }
                yInfo() << "I have received the next 3D position: ";
                yInfo() << reply.toString();
                mutexThread.lock();
                /*NextMove[0] = reply.get(0).asDouble();
                NextMove[1] = reply.get(1).asDouble();
                NextMove[2] = reply.get(2).asDouble();
                */
                Bottle *newBottle = reply.get(0).asList();

                NextMove[0] =newBottle->get(0).asDouble();
                NextMove[1] =newBottle->get(1).asDouble();
                NextMove[2] =newBottle->get(2).asDouble();
                mutexThread.unlock();
                yInfo() <<"Vector: "<< NextMove.toString();    
                statemyturn++;
            }
            case 2: // COMMAND THE PICK AND PLACE MODULE TO THE NEXT 3D POSITION
            {   yInfo() << "stateMyturn= " << statemyturn;
                
                cmd.clear();
                reply.clear();
                cmd.addString("setGoal");
                cmd.addDouble(NextMove[0]);
                cmd.addDouble(NextMove[1]);
                cmd.addDouble(NextMove[2]);
                rpcPickPlace.write(cmd,reply); //setGoal Bottle 3 doubles.
                if(reply.size () <= 0) {
                    yError() << " PickPlace empty Bottle in goal position";
                }
                else{
                    string received = reply.get(0).asString();            
                    if(received!="ack") {
                        yError() << " PickPlace doesn't receive the goal position";
                    }
                }
                cmd.clear();
                reply.clear();
                cmd.addString("pickAndPlace");
                rpcPickPlace.write(cmd,reply);
                if(reply.size () <= 0) {
                    yError() << " PickPlace empty Bottle in pickPlace task";
                }
                else{
                    string received = reply.get(0).asString();              
                    if(received!="ack") {
                        yError() << " PickPlace task not completed with sucess";
                    }
                }
                statemyturn = 0;
                myturn = false;
            }
            default:
            {
                yError() << "Something when wrong. stateMyturn= " << statemyturn;
                statemyturn=0; // Start the move ;)
                yInfo() << "Starting the move. stateMyturn= " << statemyturn;
            }
        
        
        }
    }
}
bool MasterThread::threadInit(){

    if(!openPorts())
    {
        yError("problem opening ports");
        return false;
    }
    running = true;
    clapReceived = false;
    sendCommands = true;
    statemyturn=0;
    NextMove.resize(3);
    return true;
}

bool MasterThread::openPorts()
{
    bool ret = true;
   // Open communications ports

    rpcObjRecoName="/objReco/";
    if (!rpcObjReco.open("/"+moduleName+rpcObjRecoName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", moduleName.c_str(), rpcObjRecoName.c_str());
        return false;
    }
    rpcClapName = "/clap/";
    if (!rpcClap.open("/"+moduleName+rpcClapName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", moduleName.c_str(), rpcClapName.c_str());
        return false;
    }
    rpcPlannerName = "/planner/";
    if (!rpcPlanner.open("/"+moduleName+rpcPlannerName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", moduleName.c_str(), rpcPlannerName.c_str());
        return false;
    }
    rpcPickPlaceName = "/pickandplace/";
    if (!rpcPickPlace.open("/"+moduleName+rpcPickPlaceName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", moduleName.c_str(), rpcPickPlaceName.c_str());
        return false;
    }
    rpcGameStateName = "/moveGrounding/";
    if (!rpcGameState.open("/"+moduleName+rpcGameStateName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", moduleName.c_str(), rpcGameStateName.c_str());
        return false;
    }
    rpcEmotionsName = "/emotions/";
    if (!rpcEmotions.open("/"+moduleName+rpcEmotionsName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", moduleName.c_str(), rpcEmotionsName.c_str());
        return false;
    }

    return true;
}

void MasterThread::afterStart(bool s){
    if (s) {
        yInfo() << "Master Thread started =D";
    }
    else {
        yError() << "Reading Thread did not start";
    }
}
bool MasterThread::interrupt(){
    yInfo()<< "MasterThread: " << "Interrupt ports";
    running=false;
    rpcObjReco.interrupt();
    rpcClap.interrupt();
    rpcPlanner.interrupt();
    rpcPickPlace.interrupt();
    rpcGameState.interrupt();
    rpcEmotions.interrupt();
    return true;
}
bool MasterThread::close() {
    
    yInfo () << "MasterThread: " << "closing ports";
    running=false;
    rpcObjReco.close();
    rpcClap.close();
    rpcPlanner.close();
    rpcPickPlace.close();
    rpcGameState.close();
    rpcEmotions.close();
    return true;

}
void MasterThread::threadRelease() {
    yInfo() << "I my God they kill Kenny - the Master Thread";
        //getchar();
}

