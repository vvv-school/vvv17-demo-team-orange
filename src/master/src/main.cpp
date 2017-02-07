/****************************************************************** 
**  Copyright: (C) VVV17, Santa Margherita
**  Copyright: (C) Team Orange
**  Authors: Pedro Vicente <pvicente@isr.ist.utl.pt>
**  CopyPolicy: Released under the terms of the GNU GPL v2.0.
*******************************************************************/
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>

#include "master.h"

using namespace yarp::os;
using namespace std;

int main(int argc, char *argv[]) {

    Network yarp;

    if(! yarp.checkNetwork() ) {
        yError()<<"Error: yarp server does not seem available";
        return 1;
    }

    MasterModule module;

    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultContext("vvv17");
    rf.setDefaultConfigFile("master.ini");
    rf.configure(argc, argv);
    module.runModule(rf);

    return 0;
}
