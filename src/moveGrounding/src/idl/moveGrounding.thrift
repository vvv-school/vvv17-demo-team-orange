#moveGrounding.thrift

struct Vector {}
(
    yarp.name="yarp::sig::Vector"
    yarp.includefile="yarp/sig/Vector.h"
)

service moveGrounding {
  bool init(1: list<double> boardLocation);
  bool reset();
  Vector computeNextMove(1: list<double> objLocation);
}
