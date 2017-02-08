#movePlanner.thrift

struct yMatrix {}
(
    yarp.name = "yarp::sig::Matrix"
    yarp.includefile = "yarp/sig/Matrix.h"
)

struct yVector {}
(
    yarp.name="yarp::sig::Vector"
    yarp.includefile="yarp/sig/Vector.h"
)

service movePlanner_IDL {
    /**
    * computes the next move given the updated state of the resetBoard
    * @param state a yarp::sig::Matrix with the state of the game: 0=empty, 1=icub piece, -1=opponent's piece
    * @return a vector with the (x,y) of the move icub wants to play
    */
  yVector computeNextMove(1: yMatrix state);
    /**
    * function for testing the behaviour of the movePlanner. (x,y) is the position of the opponent's last move
    */
  yVector updateNextMove(1: i32 x, 2: i32 y);
    /**
    * resets the board status to a 3x3 matrix full of zeroes
    */
  void resetBoard();
}

