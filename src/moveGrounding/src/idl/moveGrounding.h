// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_moveGrounding
#define YARP_THRIFT_GENERATOR_moveGrounding

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/sig/Vector.h>

class moveGrounding;


class moveGrounding : public yarp::os::Wire {
public:
  moveGrounding();
  virtual bool init(const std::vector<double> & boardLocation);
  virtual bool reset();
  virtual yarp::sig::Vector computeNextMove(const std::vector<double> & objLocation);
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
