/****************************************************************** 
**  Copyright: (C) VVV17, Santa Margherita
**  Copyright: (C) Team Orange
**  Authors: Pedro Vicente <pvicente@isr.ist.utl.pt>
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

    rpcPortName = "/" + moduleName + "/rpc:i";


    // open RPC server port
    if (!rpcPort.open(rpcPortName.c_str()))
    {
        yError("%s : Unable to open port %s\n", getName().c_str(), rpcPortName.c_str());
        return false;
    }
    attach(rpcPort);

    rpcObjRecoName="/objReco/";
    if (!rpcObjReco.open("/"+moduleName+rpcObjRecoName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", getName().c_str(), rpcObjRecoName.c_str());
        return false;
    }
    rpcSpeechName = "/speech/";
    if (!rpcSpeech.open("/"+moduleName+rpcSpeechName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", getName().c_str(), rpcSpeechName.c_str());
        return false;
    }
    rpcPlannerName = "/planner/";
    if (!rpcPlanner.open("/"+moduleName+rpcPlannerName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", getName().c_str(), rpcPlannerName.c_str());
        return false;
    }
    rpcPickPlaceName = "/pick/";
    if (!rpcPickPlace.open("/"+moduleName+rpcPickPlaceName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", getName().c_str(), rpcPickPlaceName.c_str());
        return false;
    }
    rpcGameStateName = "/gameState/";
    if (!rpcGameState.open("/"+moduleName+rpcGameStateName+"rpc:o"))
    {
        yError("%s : Unable to open port %s\n", getName().c_str(), rpcGameStateName.c_str());
        return false;
    }

    // Everything ok.
    yInfo() << "Everything ok";
    return true;

}

/*IDL Functions*/
bool MasterModule::quit() {
    yInfo() << "Received Quit command in RPC";
    closing = true;
    return true;
}

// To update the model externally... just if needed
bool MasterModule::update(){
    return true;
}
bool MasterModule::start(){
    starting = true;
    yInfo() << "Received Starting comming... Let's PLAY!!!";
    return true;
}
bool MasterModule::attach(RpcServer &source)
{
    return this->yarp().attachAsServer(source);

}

bool MasterModule::close() {
    yInfo() << "Calling close function";
    rpcPort.close();
    rpcObjReco.close();
    rpcSpeech.close();
    rpcPlanner.close();
    rpcPickPlace.close();
    rpcGameState.close();
    return true;
}

bool MasterModule::interruptModule() {
    yInfo() << "Calling interrupt Module function";
    rpcPort.interrupt();
    rpcObjReco.interrupt();
    rpcSpeech.interrupt();
    rpcPlanner.interrupt();
    rpcPickPlace.interrupt();
    rpcGameState.interrupt();
    return true;
}
bool MasterModule::updateModule() {
    // If nothing is connected to the master
    //yInfo() << "running";
    if(!starting || rpcObjReco.getOutputCount()<=0 || rpcSpeech.getOutputCount()<=0 || rpcPlanner.getOutputCount()<=0 || rpcPickPlace.getOutputCount()<=0 || rpcGameState.getOutputCount()<=0) {
        yInfo() << "Waiting for connections...";        
        Time::delay(0.4);        
        return !closing;
    }
    yInfo() << "Comunication Started";
    return !closing;
}

// reset the game
bool MasterModule::reset() {
    yInfo() << "Received reset command in RPC";
    return true;
}
bool MasterModule::triggerNextMove() {
    yInfo() << "Received triggerNextMove command in RPC";
    return true;
}
