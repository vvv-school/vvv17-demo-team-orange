/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <string>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>


#include <moveGrounding.h>
struct Tile {
    double x;
    double y;
    double z;
    int i;
    int j;
};

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
    virtual yarp::sig::Vector computeNextMove(const std::vector<double> & objLocation, const int32_t playerFlag);

private:
    yarp::os::RpcServer commandPort;                    // command port
    yarp::os::RpcClient requestPort;                    // command port
    std::vector <Tile> tiles;
    yarp::sig::Matrix gameState;

    bool updateState(int i, int j, int val);
};

