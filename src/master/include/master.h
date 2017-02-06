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

#include <fstream>
#include <string>

#include "MASTER_IDL.h"

using namespace yarp::os;
using namespace std;


//** IF we need a thread...

/*class Thread_read : public RateThread {
public:
    Thread_read(BufferedPort<Bottle> * broad_port,RpcClient * rpcClient,int r);
    Bottle _data,_ids;
    Mutex guard;
    Mutex guard_runit;
    bool _runit;
    virtual bool threadInit();
    
    //called by start after threadInit, s is true iff the thread started
    //successfully
    virtual void afterStart(bool s);
    virtual void threadRelease();
    virtual void run();

private:


};
*/
class MasterModule: public RFModule, public MASTER_IDL {
    string moduleName;
    
private:
    yarp::os::RpcServer rpcPort;
    yarp::os::RpcClient rpcObjReco;
    yarp::os::RpcClient rpcSpeech;
    yarp::os::RpcClient rpcPlanner;
    yarp::os::RpcClient rpcPickPlace;
public:
    
    double getPeriod();
    bool configure(yarp::os::ResourceFinder &rf);
    bool updateModule();
    bool interruptModule();
    bool close();

    // IDL functions
    bool attach(yarp::os::RpcServer &source);
    bool update();
    bool quit();
};
#endif // MASTER_H
