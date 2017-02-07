#include <yarp/os/ConstString.h>
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
    yInfo()<<"Configuring move planner..."

    return true;
}
bool movePlanner::attach(yarp::os::RpcServer &source)
{
  return this->yarp().attachAsServer(source);
}
bool movePlanner::interruptModule()
{
    if (port.asPort().isOpen()) port.interrupt();
    printf("Closing Server\n");
    return true;
}
bool movePlanner::close()
{
     printf("Calling close function\n");
    if (sManager.isRunning()) sManager.askToStop();
    if (rManager.isRunning()) rManager.stop();
    if (port.asPort().isOpen()) port.close();
    return true;
}
