#moveGrounding.thrift

struct Vector {}
(
    yarp.name="yarp::sig::Vector"
    yarp.includefile="yarp/sig/Vector.h"
)

service moveGrounding {
  bool reset();
  bool computeNextMove(1: list<Vector> objLocation, 2: Vector placeLocation);
}
