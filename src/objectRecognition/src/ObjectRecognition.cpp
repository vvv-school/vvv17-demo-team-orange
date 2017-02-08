//object detection module

#include <string>
#include <cmath>
#include <algorithm>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>

#include "ObjectRecognition.h"
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;


ObjectRecognition::ObjectRecognition() { }

ObjectRecognition::~ObjectRecognition() { }


//bool objectLocations(yarp::os::Bottle& input){



	//oRetriever.getLocation(locations,"right");

//	return false;
//}


bool ObjectRecognition::configure(yarp::os::ResourceFinder &rf) {

    yInfo()<<"Configuring the object recog module...";

    // open all ports
    bool ret = commandPort.open("/ObjectRecognition/rpc:i");
   // ret &= inPort.open("/ObjectRecognition/rpc:o");
   // ret &= outPort.open("/ObjectRecognition/out");
    if(!ret) {
        yError()<<"Cannot open some of the ports";
        return false;
    }

    if(!attach(commandPort)) {
        yError()<<"Cannot attach to the commandPort";
        return false;
    }

    // set some paramters
    modeParam = rf.check("mode", Value("coder")).asString();
    if((modeParam != "coder") && (modeParam !="decoder")) {
        yError()<<"Unknown mode value "<<modeParam;
        return false;
    }

    // everything is fine
    return true;
}



bool ObjectRecognition::updateModule() {

   /* Bottle* input = inPort.read();
    if(input==NULL)
        return false;
    std::string data;
    if(modeParam == "coder") {
        yInfo()<<"Encoding"<<input->toString();
        if(input->size())
            data = encode(input->get(0).asString());
    }
    else {
        yInfo()<<"Decoding"<<input->toString();
        if(input->size())
            data = decode(input->get(0).asString());
    }

    Bottle& output = outPort.prepare();
    output.clear();
    output.addString(data.c_str());
    outPort.write();
    */return true;
}


bool ObjectRecognition::respond(const Bottle& command, Bottle& reply) {
    yInfo()<<"Got something, echo is on";

    vector<string> objects;

    objects.push_back("one");
    objects.push_back("two"); 
    objects.push_back("three");
    objects.push_back("four");
    objects.push_back("five"); 
    objects.push_back("six");
    objects.push_back("seven");
    
        
    if (command.get(0).asString()=="quit"){
        return false;
    }
    else if (command.get(0).asString()=="return_locations")
    {  
        for (int i = 0; i < objects.size(); i++)
        {
            yInfo()<<"returning locations";

            if (objRet.getLocation(locations, objects.at(i)))
            {
                reply.clear();
                reply.addDouble(locations[0]);
                reply.addDouble(locations[1]);
                reply.addDouble(locations[2]);

                return true;
            }
            else{
                yInfo() << "Could not return locations";
                return false;
            }
        }


    }else {
        reply.clear();
        reply.addString("error");
    }
    
    return true;
}


bool ObjectRecognition::interruptModule() {
//    yInfo()<<"Interrupting module";
 //   inPort.interrupt();
    return true;
}


bool ObjectRecognition::close() {
    yInfo()<<"closing module";
    commandPort.close();
  //  inPort.close();
    // you can force writing remaining data
    // using outPort.writeStrict();
 //   outPort.close();
    return true;
}

