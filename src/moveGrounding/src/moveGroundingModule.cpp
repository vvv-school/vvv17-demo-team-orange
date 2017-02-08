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
    bool ret = commandPort.open("/moveGroundingModule/rpc:i");
    ret &= requestPort.open("/moveGroundingModule/rpc:o");

    if(!ret) {
        yError()<<"Cannot open some of the ports";
        return false;
    }

    if(!yarp().attachAsServer(commandPort)) {
        yError()<<"Cannot attach to the commandPort";
        return false;
    }

    if(!iMovePlanner.yarp().attachAsClient(requestPort)) {
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
    yInfo() << "Resetting";
    std::vector<double > boardCorners;
    boardCorners.push_back(tiles[0].x);
    boardCorners.push_back(tiles[0].y);
    boardCorners.push_back(tiles[0].z);
    boardCorners.push_back(tiles[8].x);
    boardCorners.push_back(tiles[8].y);
    boardCorners.push_back(tiles[8].z);
    gameState.zero();
    gameState.resize(3,3);
    gameState.zero();
    init(boardCorners);

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
     if (!retrievePointFromList(boardLocation,0,topLeft)) {
         yInfo() << "Could not retrieve top left corner";
         return false;
     }
    if (!retrievePointFromList(boardLocation,3,bottomRight)) {
        yInfo() << "Could not retrieve bottom right corner";
        return false;
    }

    yInfo() << "Retrieved topLeft  = [ " << topLeft[0] << ", " << topLeft[1] << ", " << topLeft[2] << "]" ;
    yInfo() << "Retrieved bottomRight = [ " << bottomRight[0] << ", " << bottomRight[1] << ", " << bottomRight[2] << "]" ;

    tiles.resize(9);
    int count = 0;
    double boardHeight = bottomRight[0] - topLeft[0];
    double boardWidth = bottomRight[1] - topLeft[1];
    double tileHeight = boardHeight/2;
    double tileWidth = boardWidth/2;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Tile currTile;
            currTile.x = topLeft[0] + i * tileHeight;
            currTile.y = topLeft[1] + j * tileWidth;
            currTile.z= topLeft[2];
            currTile.i = i;
            currTile.j = j;
            tiles[count] = currTile;
            count ++;
            yInfo() << "Tile location "<< count <<" = [ " << currTile.x << ", " << currTile.y << ", " << currTile.z << "]" ;
        }
    }
    return true;
};

bool moveGroundingModule::updateState(int i, int j, int val) {
    if (gameState(i,j) == val) {
        return false;
    } else if (gameState(i,j) != 0 || gameState(i,j) == -val){
        yInfo() << "Trying to update state to invalid location";
        return false;
    } else {
        gameState(i,j) = val;
        return true;
    }
}

yarp::sig::Vector moveGroundingModule::askNextMove(){
    yarp::sig::Vector tileCoordinates = iMovePlanner.computeNextMove(gameState);
    std::cout << "tileCoordinates.size() = " << tileCoordinates.size() << std::endl;
    if (tileCoordinates.size() == 0){
        yInfo() << "Could not retrieve next move. Check connection with movePlanner";
        return tileCoordinates;
    }
    if (tileCoordinates.size() == 1){
        yInfo() << "GAME OVER";
        return tileCoordinates;
    }

    for (int i = 0; i < tiles.size(); ++i) {
        if (tiles[i].i == tileCoordinates[0] && tiles[i].j == tileCoordinates[1]){
            yarp::sig::Vector tileLocation(3);
            tileLocation[0] = tiles[i].x;
            tileLocation[1] = tiles[i].y;
            tileLocation[2] = tiles[i].z;
            yInfo() << "Retrieved next move location = [ " << tileLocation[0] << ", " << tileLocation[1] << ", " << tileLocation[2] << "]" ;
            updateState(tiles[i].i, tiles[i].j, 1);
            return tileLocation;
        }
    }


}

yarp::sig::Vector moveGroundingModule::computeNextMove(const std::vector<double> & objLocation, const int32_t playerFlag){

    if (objLocation.size() == 0){
        return askNextMove();
    }
    yInfo() << "Grounding!";
    yarp::sig::Vector placeLocation(3);
    yarp::sig::Vector point (3);
    yarp::sig::Vector tileLocation (3);
    Tile tile;
    Tile closestTile;
    int i = 0;
    double dist = 0;
    bool stateUpdated = false;

    while (retrievePointFromList(objLocation,i,point))
    {
        double minDist = numeric_limits<double>::max();

        for (int j = 0; j < tiles.size(); ++j) {
            tile = tiles[j];
            tileLocation.clear();
            tileLocation.resize(3);
            tileLocation[0] = tile.x;
            tileLocation[1] = tile.y;
            tileLocation[2] = tile.z;
            dist =  norm2(point - tileLocation);
            if (minDist > dist){
                minDist = dist;
                closestTile = tile;
            }
        }
        stateUpdated |= updateState(closestTile.i, closestTile.j, playerFlag);
        i += 3;
        yInfo() << "Closest tile = " << closestTile.i << ", " << closestTile.j;
        yInfo() << "Retrieved point " << i/3 << " = [ " << point[0] << ", " << point[1] << ", " << point[2] << "]" ;

    }


    if (stateUpdated){
        yInfo() << "State Updated";
        if (playerFlag == -1){
            placeLocation = askNextMove();
        } else{
            placeLocation[0] = closestTile.x;
            placeLocation[1] = closestTile.y;
            placeLocation[2] = closestTile.z;
        }
    } else{
        yInfo() << "State not updated";
        placeLocation.clear();
        placeLocation.push_back(0);
    }

    yInfo()<< "Game state = \n";
    for (int k = 0; k < 3; ++k) {
        std::cout << "[ ";
        for (int j = 0; j < 3; ++j) {
            std::cout << gameState(k,j) << ", ";
        }
        std::cout << " ]\n";
    }
    return placeLocation;

};
