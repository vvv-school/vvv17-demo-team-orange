#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/RpcServer.h>
#include <movePlanner_IDL.h>

class movePlanner:public yarp::os::RFModule,
                        public movePlanner_IDL
{
    int  CNT;
    yarp::sig::Matrix       board;
    yarp::os::Mutex         mutex;
    yarp::os::RpcServer     RPCPort;


public:

    movePlanner(){}

    ~movePlanner(){}

    double getPeriod();

    // This is our main function. Will be called periodically every getPeriod() seconds
    bool updateModule();

    // Configure function. Receive a previously initialized
    // resource finder object.
    bool configure(yarp::os::ResourceFinder &rf);

    // Function needed for the thrift generated yarp connectivity
    bool attach(yarp::os::RpcServer &source);

    // Interrupt function.
    bool interruptModule();

    // Close function, to perform cleanup.
    bool close();
    
    // Function available at the RPC port
    yarp::sig::Vector computeNextMove(yarp::sig::Matrix state);

private:
    // updates the game state, can accept the position of the move as an idex of
    // the vectorized matrix
    bool updateGameState(int x, int y = -1);

    // check if the game is over
    bool checkEnd();

};
