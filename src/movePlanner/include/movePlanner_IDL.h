// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_movePlanner_IDL
#define YARP_THRIFT_GENERATOR_movePlanner_IDL

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class movePlanner_IDL;


class movePlanner_IDL : public yarp::os::Wire {
public:
  movePlanner_IDL();
  virtual int32_t get_answer(const int32_t oppMove);
  virtual bool reset();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
