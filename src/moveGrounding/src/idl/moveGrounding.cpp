// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <moveGrounding.h>
#include <yarp/os/idl/WireTypes.h>



class moveGrounding_reset : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class moveGrounding_computeNextMove : public yarp::os::Portable {
public:
  std::vector<yarp::sig::Vector>  objLocation;
  yarp::sig::Vector placeLocation;
  bool _return;
  void init(const std::vector<yarp::sig::Vector> & objLocation, const yarp::sig::Vector& placeLocation);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool moveGrounding_reset::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("reset",1,1)) return false;
  return true;
}

bool moveGrounding_reset::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void moveGrounding_reset::init() {
  _return = false;
}

bool moveGrounding_computeNextMove::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("computeNextMove",1,1)) return false;
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(objLocation.size()))) return false;
    std::vector<yarp::sig::Vector> ::iterator _iter0;
    for (_iter0 = objLocation.begin(); _iter0 != objLocation.end(); ++_iter0)
    {
      if (!writer.writeNested((*_iter0))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  if (!writer.write(placeLocation)) return false;
  return true;
}

bool moveGrounding_computeNextMove::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void moveGrounding_computeNextMove::init(const std::vector<yarp::sig::Vector> & objLocation, const yarp::sig::Vector& placeLocation) {
  _return = false;
  this->objLocation = objLocation;
  this->placeLocation = placeLocation;
}

moveGrounding::moveGrounding() {
  yarp().setOwner(*this);
}
bool moveGrounding::reset() {
  bool _return = false;
  moveGrounding_reset helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool moveGrounding::reset()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool moveGrounding::computeNextMove(const std::vector<yarp::sig::Vector> & objLocation, const yarp::sig::Vector& placeLocation) {
  bool _return = false;
  moveGrounding_computeNextMove helper;
  helper.init(objLocation,placeLocation);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool moveGrounding::computeNextMove(const std::vector<yarp::sig::Vector> & objLocation, const yarp::sig::Vector& placeLocation)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool moveGrounding::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "reset") {
      bool _return;
      _return = reset();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "computeNextMove") {
      std::vector<yarp::sig::Vector>  objLocation;
      yarp::sig::Vector placeLocation;
      {
        objLocation.clear();
        uint32_t _size1;
        yarp::os::idl::WireState _etype4;
        reader.readListBegin(_etype4, _size1);
        objLocation.resize(_size1);
        uint32_t _i5;
        for (_i5 = 0; _i5 < _size1; ++_i5)
        {
          if (!reader.readNested(objLocation[_i5])) {
            reader.fail();
            return false;
          }
        }
        reader.readListEnd();
      }
      if (!reader.read(placeLocation)) {
        reader.fail();
        return false;
      }
      bool _return;
      _return = computeNextMove(objLocation,placeLocation);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "help") {
      std::string functionName;
      if (!reader.readString(functionName)) {
        functionName = "--all";
      }
      std::vector<std::string> _return=help(functionName);
      yarp::os::idl::WireWriter writer(reader);
        if (!writer.isNull()) {
          if (!writer.writeListHeader(2)) return false;
          if (!writer.writeTag("many",1, 0)) return false;
          if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(_return.size()))) return false;
          std::vector<std::string> ::iterator _iterHelp;
          for (_iterHelp = _return.begin(); _iterHelp != _return.end(); ++_iterHelp)
          {
            if (!writer.writeString(*_iterHelp)) return false;
           }
          if (!writer.writeListEnd()) return false;
        }
      reader.accept();
      return true;
    }
    if (reader.noMore()) { reader.fail(); return false; }
    yarp::os::ConstString next_tag = reader.readTag();
    if (next_tag=="") break;
    tag = tag + "_" + next_tag;
  }
  return false;
}

std::vector<std::string> moveGrounding::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("reset");
    helpString.push_back("computeNextMove");
    helpString.push_back("help");
  }
  else {
    if (functionName=="reset") {
      helpString.push_back("bool reset() ");
    }
    if (functionName=="computeNextMove") {
      helpString.push_back("bool computeNextMove(const std::vector<yarp::sig::Vector> & objLocation, const yarp::sig::Vector& placeLocation) ");
    }
    if (functionName=="help") {
      helpString.push_back("std::vector<std::string> help(const std::string& functionName=\"--all\")");
      helpString.push_back("Return list of available commands, or help message for a specific function");
      helpString.push_back("@param functionName name of command for which to get a detailed description. If none or '--all' is provided, print list of available commands");
      helpString.push_back("@return list of strings (one string per line)");
    }
  }
  if ( helpString.empty()) helpString.push_back("Command not found");
  return helpString;
}


