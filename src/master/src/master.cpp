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

    string nameStarts = rf.check("whostarts", Value("robot"),"Who starts the game? (human/robot) (string)").asString();
    
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
    yInfo() <<"MasterModule: " << "DONE?!";
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
        if(rpcObjReco.getOutputCount()<=0) {// || rpcSpeech.getOutputCount()<=0 || rpcPlanner.getOutputCount()<=0 || rpcPickPlace.getOutputCount()<=0 || rpcGameState.getOutputCount()<=0) {
            yInfo() << "MasterThread: Waiting for connections...";        
            Time::delay(0.4);  
            return;      
        }
        yInfo() << "MasterThread: Comunication Started";
        Bottle cmd,reply;
        cmd.addDouble(1);
        rpcObjReco.write(cmd,reply);
        


        stateMachine();
    }
}

void MasterThread::stateMachine() {

}
bool MasterThread::threadInit(){

    if(!openPorts())
    {
        yError("problem opening ports");
        return false;
    }
    running = true;
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
    if (!rpcSpeech.open("/"+moduleName+rpcClapName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", moduleName.c_str(), rpcSpeechName.c_str());
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
    rpcGameStateName = "/gameState/";
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
    else
        yError() << "Reading Thread did not start";
}
bool MasterThread::interrupt(){
    yInfo()<< "MasterThread: " << "Interrupt ports";
    running=false;
    rpcObjReco.interrupt();
    rpcSpeech.interrupt();
    rpcPlanner.interrupt();
    rpcPickPlace.interrupt();
    rpcGameState.interrupt();
    return true;
}
bool MasterThread::close() {
    
    yInfo () << "MasterThread: " << "closing ports";
    running=false;
    rpcObjReco.close();
    rpcSpeech.close();
    rpcPlanner.close();
    rpcPickPlace.close();
    rpcGameState.close();
    return true;

}
void MasterThread::threadRelease() {
    yInfo() << "I my God they kill Kenny - the Master Thread";
        //getchar();
}

