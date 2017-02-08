/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <moveGroundingModule.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Math.h>
#include <cmath>
#include <limits>

using namespace std;
using namespace yarp::os;
using namespace yarp::math;



moveGroundingModule::moveGroundingModule() { }

moveGroundingModule::~moveGroundingModule() { }


bool moveGroundingModule::configure(yarp::os::ResourceFinder &rf) {

    yInfo()<<"Opening grounding module";

    gameState.resize(3,3);
    gameState.zero();

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

    if(!yarp().attachAsClient(requestPort)) {
        yError()<<"Cannot attach to the requestPort";
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
    gameState.zero();
    tiles.clear();
    tiles.resize(9);
    gameState.resize(3,3);


    return true;
};

bool retrievePointFromList(std::vector<double> list, int startIdx, yarp::sig::Vector & point){
    point.resize(3);
    point.zero();
    for (int i = startIdx; i < startIdx + 3; ++i) {
        if (i > (list.size() -1))
            return false;
        point[i - startIdx] = list[i];
    }
    return true;
}

bool moveGroundingModule::init(const std::vector<double> &boardLocation){
    yarp::sig::Vector topLeft(3);
    yarp::sig::Vector bottomRight(3);
     if (!retrievePointFromList(boardLocation,0,topLeft))
        return false;
    if (!retrievePointFromList(boardLocation,3,bottomRight))
        return false;

    double boardSize = (std::fabs(topLeft[0] - bottomRight[0]) + std::fabs(topLeft[0] - bottomRight[0]))/2;
    double tileSize = boardSize/3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Tile currTile = tiles[i*j];
            currTile.x = topLeft[0] + i * tileSize;
            currTile.y = topLeft[1] - j * tileSize;
            currTile.z= topLeft[2];
            currTile.i = i;
            currTile.j = j;
        }
    }
    return true;
};

bool moveGroundingModule::updateState(int i, int j, int val) {
    if (gameState(i,j) == val) {
        return false;
    } else if (!gameState(i,j) || gameState(i,j) == -val){
        yInfo() << "Trying to update state to invalid location";
        return false;
    } else {
        gameState(i,j) = val;
        return true;
    }
}

yarp::sig::Vector askNextMove(){
    yarp::os::Bottle cmd,reply;
    cmd.addString("computeNextMove");
    //TODO
}

yarp::sig::Vector moveGroundingModule::computeNextMove(const std::vector<double> & objLocation, const int32_t playerFlag){
    yarp::sig::Vector placeLocation(objLocation.size());
    yarp::sig::Vector point (3);
    yarp::sig::Vector tileLocation (3);
    Tile tile;
    Tile closestTile;
    int i = 0;
    double dist = 0;
    double minDist = numeric_limits<double>::max();
    bool stateUpdated = false;

    while (retrievePointFromList(objLocation,i,point))
    {
        for (int j = 0; j < tiles.size(); ++j) {
            tile = tiles[j];
            tileLocation.clear();
            tileLocation[0] = tile.x;
            tileLocation[1] = tile.y;
            tileLocation[2] = tile.z;
            dist =  norm(point - tileLocation);
            if (minDist > dist){
                minDist = dist;
                closestTile = tile;
            }
            stateUpdated |= updateState(closestTile.i, closestTile.j, playerFlag);
        }
        i += 3;
    }

    if (stateUpdated){
        if (playerFlag == -1){
            placeLocation = askNextMove();
        } else{
            placeLocation[0] = closestTile.x;
            placeLocation[1] = closestTile.y;
            placeLocation[2] = closestTile.z;
        }
    } else{
        placeLocation.clear();
    }


    return placeLocation;

};
