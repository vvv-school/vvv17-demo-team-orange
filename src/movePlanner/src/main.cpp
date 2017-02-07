/****************************************************************** 
**  Copyright: (C) VVV17, Santa Margherita
**  Copyright: (C) Team Orange
**  Authors: Gian Maria Marconi <gianmaria.marconi@iit.it>
**  CopyPolicy: Released under the terms of the GNU GPL v2.0.
*******************************************************************/
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>

#include "movePlanner.h"

using namespace yarp::os;
using namespace std;

int main(int argc, char *argv[]) {

    Network yarp;

    if(! yarp.checkNetwork() ) {
        fprintf(stdout,"Error: yarp server does not seem available\n");
        return 1;
    }

    movePlanner module;

    ResourceFinder rf;
    module.runModule(rf);

    return 0;
}
