#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include "ObjectRecognition.h"
using namespace yarp::os;


int main(int argc, char *argv[]) 
{
    Network yarp;

    ObjectRecognition objRec;
    //ObjectRetriever objRet;
    //RpcServer outPort;
    //RpcServer inPort;

    //outPort.open("/objLocation:o");
   	//inPort.open("/objLocation/rpc:i");

    ResourceFinder rf;
    rf.configure(argc, argv);
    rf.setDefaultContext("tutorial_RFModule");
    //rf.setDefaultConfigFile("tutorial_RFModule.ini");
    // rf.setVerbose(true);

    objRec.runModule(rf);

    yInfo()<<"Main returning...";
    return 0;
}
