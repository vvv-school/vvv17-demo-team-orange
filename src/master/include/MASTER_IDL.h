// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_MASTER_IDL
#define YARP_THRIFT_GENERATOR_MASTER_IDL

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class MASTER_IDL;


class MASTER_IDL : public yarp::os::Wire {
public:
  MASTER_IDL();
  virtual bool update();
  virtual bool reset();
  virtual bool quit();
  virtual bool triggerNextMove();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
