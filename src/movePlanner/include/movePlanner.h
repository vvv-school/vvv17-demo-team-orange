#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <yarp/sig/Matrix.h>
#include <movePlanner_IDL.h>
#include <yarp/os/all.h>
#include <math.h>       /* floor */

class movePlanner:public yarp::os::RFModule,
                        public movePlanner_IDL
{
    bool                    dbg_flag;;
    int                     winner; // 1=icub, -1=opponent
    //int                     x, y, lindex;
    yarp::os::ConstString   policy;
    yarp::os::ConstString   portname;
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
    yarp::sig::Vector computeNextMove(const yarp::sig::Matrix &boardupdate);
    
    // overloading the 
    yarp::sig::Vector updateNextMove( int x, int y);
    
    // resets the state
    void resetBoard();

private:
    
    yarp::sig::Vector freePlaces;
    
    // check if the game is over
    bool checkEnd();
    
    // computes a random valid move
    yarp::sig::Vector randomPolicy();
    
    // plots to debug the board state
    void debugPlotState();
    
    //void lindex2xy(int lindex, int& x, int& y);
    
    //void xy2lindex(int x, int y, int& lindex);

};
