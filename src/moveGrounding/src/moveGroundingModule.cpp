/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <moveGroundingModule.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;



moveGroundingModule::moveGroundingModule() { }

moveGroundingModule::~moveGroundingModule() { }


bool moveGroundingModule::configure(yarp::os::ResourceFinder &rf) {

    yInfo()<<"Opening grounding module";

    // open all ports
    bool ret = commandPort.open("/moveGroundingModule/rpc");
    if(!ret) {
        yError()<<"Cannot open some of the ports";
        return false;
    }

    if(!yarp().attachAsServer(commandPort)) {
        yError()<<"Cannot attach to the commandPort";
        return false;
    }

    // everything is fine
    return true;
}


double moveGroundingModule::getPeriod() {
    return 2.0; // module periodicity (seconds)
}


bool moveGroundingModule::updateModule() {
    yInfo()<<"moveGroundingModule running happily...";
    return true;
}



bool moveGroundingModule::close() {
    yInfo()<<"closing moveGrounding module";
    commandPort.close();
    return true;
}

bool moveGroundingModule::reset(){
    return true;
};

bool moveGroundingModule::computeNextMove(const std::vector<yarp::sig::Vector> &objLocation,
                                          const yarp::sig::Vector &placeLocation) {
    return true;
}