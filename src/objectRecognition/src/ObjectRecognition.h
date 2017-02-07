//object detection header

#include <string>
#include <yarp/os/PortReport.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/RFModule.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include "helpers.h"


class ObjectRecognition : public yarp::os::RFModule
{
public:
	
    ObjectRecognition();
    virtual ~ObjectRecognition();

	//bool ObjectRecognition(yarp::os::Bottle& input);

/*
    * Configure function. Receive a previously initialized
    * resource finder object. Use it to configure your module.
    * Open port and attach it to message handler and etc.
    */
    virtual bool configure(yarp::os::ResourceFinder &rf);


    /*
    * This is our main function. Will be called periodically every getPeriod() seconds.
    */
    virtual bool updateModule();

    /*
    * Message handler. Just echo all received messages.
    */
    virtual bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);

    /*
    * Interrupt function.
    */
    virtual bool interruptModule();

    /*
    * Close function, to perform cleanup.
    */
    virtual bool close();


private:
    std::string modeParam;

    yarp::os::RpcServer commandPort;                    // command port
 //   yarp::os::BufferedPort<yarp::os::Bottle> inPort;    // input port
 //   yarp::os::BufferedPort<yarp::os::Bottle> outPort;   // output port
	ObjectRetriever objRet;
    yarp::os::Bottle outputBottle; 
//	yarp::os::RpcClient outPort;
//    yarp::os::RpcClient inPort;
 //   ObjectRetriever oRetriever; 
    yarp::sig::Vector locations;

};