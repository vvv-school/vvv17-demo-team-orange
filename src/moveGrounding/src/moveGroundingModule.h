/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <string>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>


#include <idl/moveGrounding.h>


class moveGroundingModule : public yarp::os::RFModule, public moveGrounding
{
public:

    moveGroundingModule();
    virtual ~moveGroundingModule();

    virtual bool configure(yarp::os::ResourceFinder &rf);
    virtual double getPeriod();
    virtual bool updateModule();
    virtual bool close();

    // Memory interface see Memory.thrift
    virtual bool reset();
    virtual bool init(const std::vector<double> &boardLocation);
    virtual yarp::sig::Vector computeNextMove(const std::vector<double> &objLocation);

private:
    yarp::os::RpcServer commandPort;                    // command port
};

