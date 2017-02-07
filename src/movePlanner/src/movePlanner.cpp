#include <yarp/os/ConstString.h>
#include <yarp/os/LogStream.h>
#include "movePlanner.h"

using namespace std;
using namespace yarp::os;

double movePlanner::getPeriod()
{
    // module periodicity (seconds), called implicitly by the module.
    return 0.2;
}
bool movePlanner::updateModule()
{
    return true;
}
bool movePlanner::configure(yarp::os::ResourceFinder &rf)
{
    yInfo()<<"Configuring move planner...";
    //implement port opening and attach
    if (!RPCPort.open("/movePlanner/rpc:i"))
    {
        yError()<<"Cannot open port";
        return false;
    }
     
    if (!attach(RPCPort))
        return false;

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
yarp::sig::Vector movePlanner::computeNextMove(yarp::sig::Matrix state)
{
    
}
