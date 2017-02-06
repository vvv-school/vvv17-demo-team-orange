/****************************************************************** 
**  Copyright: (C) VVV17, Santa Margherita
**  Copyright: (C) Team Orange
**  Authors: Pedro Vicente <pvicente@isr.ist.utl.pt>
**  CopyPolicy: Released under the terms of the GNU GPL v2.0.
*******************************************************************/

#include "master.h"

using namespace yarp::os;
using namespace std;

double MasterModule::getPeriod() {
    return 0.0;
}

/*IDL Functions*/
bool MasterModule::quit() {
    cout << "Received Quit command in RPC" << endl;
    return true;
}

bool MasterModule::update(){
}

bool MasterModule::configure(yarp::os::ResourceFinder &rf) {
}

bool MasterModule::attach(RpcServer &source)
{
    return this->yarp().attachAsServer(source);
}

bool MasterModule::close() {
}

bool MasterModule::interruptModule() {
}
bool MasterModule::updateModule() {
}
