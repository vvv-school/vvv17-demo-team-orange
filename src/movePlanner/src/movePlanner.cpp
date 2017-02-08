#include <yarp/os/ConstString.h>
#include <yarp/os/LogStream.h>
#include "movePlanner.h"

using namespace std;
using namespace yarp::os;

double movePlanner::getPeriod()
{
    // module periodicity (seconds), called implicitly by the module.
    return 30;
}
bool movePlanner::updateModule()
{
    if (dbg_flag) yDebug()<<"movePlanner: waiting for new board state";
    return true;
}
bool movePlanner::configure(yarp::os::ResourceFinder &rf)
{    
    yInfo()<<"Configuring move planner...";
    policy   = rf.check("policy", Value("random"), "Getting game policy").asString();
    portname = rf.check("RPCname", Value("/movePlanner/rpc:i"), "Getting RPC port name").asString();
    dbg_flag = rf.check("debug", yarp::os::Value(false), "Getting debug flag").asBool();
    dbg_flag = true;
    //implement port opening and attach
    if (!RPCPort.open(portname))
    {
        yError()<<"Cannot open port";
        return false;
    }
     
    if (!attach(RPCPort))
        return false;
    
    board.resize(3,3);
    board.zero();

    return true;
}
bool movePlanner::attach(yarp::os::RpcServer &source)
{
  return yarp().attachAsServer(source);
}
bool movePlanner::interruptModule()
{
    if (RPCPort.asPort().isOpen()) RPCPort.interrupt();
    printf("Closing Server\n");
    return true;
}
bool movePlanner::close()
{
    printf("Calling close function\n");
    if (RPCPort.asPort().isOpen()) RPCPort.close();
    return true;
}
bool movePlanner::checkEnd()
{
    int sign = 1;
    if (
        // horizontal combinations
        ( (board(0,0)==sign) && (board(0,1)==sign) && (board(0,2)==sign) ) ||
        ( (board(1,0)==sign) && (board(1,1)==sign) && (board(1,2)==sign) ) ||
        ( (board(2,0)==sign) && (board(2,1)==sign) && (board(2,2)==sign) ) ||
        // vertical combinations
        ( (board(0,0)==sign) && (board(1,0)==sign) && (board(2,0)==sign) ) ||
        ( (board(0,1)==sign) && (board(1,1)==sign) && (board(2,1)==sign) ) ||
        ( (board(0,2)==sign) && (board(1,2)==sign) && (board(2,2)==sign) ) ||
        // diagonals
        ( (board(0,0)==sign) && (board(1,1)==sign) && (board(2,2)==sign) ) ||
        ( (board(0,2)==sign) && (board(1,1)==sign) && (board(2,0)==sign) )
        )
    {
        winner = sign;
        return true;
    }
    sign = -1;
    if (
        // horizontal combinations
        ( (board(0,0)==sign) && (board(0,1)==sign) && (board(0,2)==sign) ) ||
        ( (board(1,0)==sign) && (board(1,1)==sign) && (board(1,2)==sign) ) ||
        ( (board(2,0)==sign) && (board(2,1)==sign) && (board(2,2)==sign) ) ||
        // vertical combinations
        ( (board(0,0)==sign) && (board(1,0)==sign) && (board(2,0)==sign) ) ||
        ( (board(0,1)==sign) && (board(1,1)==sign) && (board(2,1)==sign) ) ||
        ( (board(0,2)==sign) && (board(1,2)==sign) && (board(2,2)==sign) ) ||
        // diagonals
        ( (board(0,0)==sign) && (board(1,1)==sign) && (board(2,2)==sign) ) ||
        ( (board(0,2)==sign) && (board(1,1)==sign) && (board(2,0)==sign) )
        )
    {
        winner = sign; 
        return true;
    }
    return false;
    
}
yarp::sig::Vector movePlanner::computeNextMove(const yarp::sig::Matrix & boardupdate)
{

    int diff = 0;
    for (int idx=0; idx < 3; idx++){
        for (int jdx=0; jdx < 3; jdx++){
            if (board(idx,jdx) != boardupdate(idx,jdx))
                diff++;
        }
    }
    if (diff > 1)
        yError()<<"Invalid board state";
            
    mutex.lock();
    board = boardupdate;
    mutex.unlock();
    yarp::sig::Vector returnMove(0);
    if (checkEnd())
    {
        returnMove.push_back(winner*42);
        if (dbg_flag)
            debugPlotState();
        resetBoard();
        return returnMove;
    }
    if (policy == "random") returnMove = randomPolicy(); 
    
    return returnMove;
}

yarp::sig::Vector movePlanner::updateNextMove( int x, int y)
{
    if ( (x>2) || (x<0) || (y>2) || (y<0) )
    {
        yError()<<"movePlanner: A in valid position was requested x="<<x<<" y="<<y;
        yarp::sig::Vector v(1,404);
        return v;
    }
    mutex.lock();
    board(x,y) = -1;
    mutex.unlock();
    yarp::sig::Vector m = computeNextMove(board);
    if (dbg_flag)
        yDebug()<<"movePlanner: chosen move is x="<<m[0]<<" y="<<m[1];
    return m;
}


yarp::sig::Vector movePlanner::randomPolicy()
{
    Random::seed(Time::now());
    int x, y;
    freePlaces.clear();
    for (int idx = 0; idx < 9; idx++)
    {
        x = floor(idx / 3);
        y = idx % 3;
        if (board(x,y) == 0)
        {
            freePlaces.push_back(x);
            freePlaces.push_back(y);
        }
    }
    int n = freePlaces.length() / 2;
    if ( (n % 2)  != 0 ) yDebug()<<"moveplanner: somethins is wrong inside randomPolicy";
    if ( n == 0 ) yError()<<"movePlanner: trying to make a move on a full board";
    int index = floor(Random::uniform(0,n));
    index *= 2;
    
    yarp::sig::Vector nextMove;
    x = freePlaces[index];
    y = freePlaces[index+1];
    freePlaces.clear();
    nextMove.clear();
    nextMove.push_back( x );
    nextMove.push_back( y );
    mutex.lock();
    board(x,y) = 1;
    mutex.unlock();
    if (dbg_flag)
        debugPlotState();
    return nextMove;
}

void movePlanner::debugPlotState()
{
    for (int idx = 0; idx < 3; idx++)
    {
        yDebug()<<board(idx,0)<<"||"<<board(idx,1)<<"||"<<board(idx,2);
    }
}

void movePlanner::resetBoard()
{
    mutex.lock();
    board.zero();
    mutex.unlock();
    winner = 0;
    yInfo()<<"The board has been resetted";
}