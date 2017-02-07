/****************************************************************** 
**  Copyright: (C) VVV17, Santa Margherita
**  Copyright: (C) Team Orange
**  Authors: Pedro Vicente <pvicente@isr.ist.utl.pt>
**  CopyPolicy: Released under the terms of the GNU GPL v2.0.
*******************************************************************/

// In case we need to reply with Bottles
struct Bottle { }
(
yarp.name = "yarp::os::Bottle"
yarp.includefile="yarp/os/Bottle.h"
)

service MASTER_IDL
{
  bool update();
  bool reset();
  bool quit();
  bool start();
  bool triggerNextMove();  
}
