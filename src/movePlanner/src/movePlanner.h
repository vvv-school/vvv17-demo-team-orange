#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/sig/Matrix.h>
#include <../include/movePlanner_IDL.h>

class movePlanner:public yarp::os::RFModule,
                        public movePlanner_IDL
{
    int  count;
    int  secs;
    int  CNT;
    yarp::sig::Matrix       board;
    yarp::os::Mutex         mutex;
    yarp::os::RpcServer     RPCPort;
    yarp::os::Network       yarp;

public:

    movePlanner(){}

    ~movePlanner(){}

    double getPeriod();

    // This is our main function. Will be called periodically every getPeriod() seconds
    bool updateModule();

    // Configure function. Receive a previously initialized
    // resource finder object.
    bool configure(yarp::os::ResourceFinder &rf);

    // Message handler. Takes the opponent's move in to update the internal state
    bool respond(const yarp::os::Bottle& update, yarp::os::Bottle& reply);

    // Function needed for the thrift generated yarp connectivity
    bool attach(yarp::os::RpcServer &source)

    // Interrupt function.
    bool interruptModule();

    // Close function, to perform cleanup.
    bool close();

private:
    // updates the game state, can accept the position of the move as an idex of
    // the vectorized matrix
    bool updateGameState(int x, int y = -1);

    // check if the game is over
    bool checkEnd();

};
