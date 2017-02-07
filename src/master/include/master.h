/****************************************************************** 
**  Copyright: (C) VVV17, Santa Margherita
**  Copyright: (C) Team Orange
**  Authors: Pedro Vicente <pvicente@isr.ist.utl.pt>
**  CopyPolicy: Released under the terms of the GNU GPL v2.0.
*******************************************************************/

#ifndef MASTER_H
#define MASTER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <fstream>
#include <string>

#include "MASTER_IDL.h"

using namespace yarp::os;
using namespace std;


class MasterThread : public RateThread {
public:
    MasterThread(string Name,int r);
    string moduleName;
    
    Mutex guard;
    virtual bool threadInit();
    
    //called by start after threadInit, s is true iff the thread started
    //successfully
    virtual void afterStart(bool s);
    virtual void threadRelease();
    virtual void run();
    bool interrupt();
    bool close();
    bool running;
    

private:

    yarp::os::RpcClient rpcObjReco;
    yarp::os::RpcClient rpcSpeech;
    yarp::os::RpcClient rpcPlanner;
    yarp::os::RpcClient rpcPickPlace;
    yarp::os::RpcClient rpcGameState;

    string rpcObjRecoName;
    string rpcSpeechName;
    string rpcPlannerName;
    string rpcPickPlaceName;
    string rpcGameStateName;
    void stateMachine();
    bool openPorts();

};

class MasterModule: public RFModule, public MASTER_IDL {
    string moduleName;
    
private:
    yarp::os::RpcServer rpcPort;

    string rpcPortName;

    bool closing;
    bool starting;
    double threadPeriod;

    //
public:
    MasterThread *comunThread;
    double getPeriod();
    bool configure(yarp::os::ResourceFinder &rf);
    bool updateModule();
    bool interruptModule();
    bool close();

    // IDL functions
    bool attach(yarp::os::RpcServer &source);
    bool update();
    bool quit();
    bool start();
    bool reset();
    bool triggerNextMove();
};
#endif // MASTER_H
