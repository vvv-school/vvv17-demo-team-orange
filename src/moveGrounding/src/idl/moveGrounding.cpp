// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <moveGrounding.h>
#include <yarp/os/idl/WireTypes.h>



class moveGrounding_init : public yarp::os::Portable {
public:
  std::vector<double>  boardLocation;
  bool _return;
  void init(const std::vector<double> & boardLocation);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class moveGrounding_reset : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class moveGrounding_computeNextMove : public yarp::os::Portable {
public:
  std::vector<double>  objLocation;
  yarp::sig::Vector _return;
  void init(const std::vector<double> & objLocation);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool moveGrounding_init::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("init",1,1)) return false;
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(boardLocation.size()))) return false;
    std::vector<double> ::iterator _iter0;
    for (_iter0 = boardLocation.begin(); _iter0 != boardLocation.end(); ++_iter0)
    {
      if (!writer.writeDouble((*_iter0))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}

bool moveGrounding_init::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void moveGrounding_init::init(const std::vector<double> & boardLocation) {
  _return = false;
  this->boardLocation = boardLocation;
}

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
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("computeNextMove",1,1)) return false;
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(objLocation.size()))) return false;
    std::vector<double> ::iterator _iter1;
    for (_iter1 = objLocation.begin(); _iter1 != objLocation.end(); ++_iter1)
    {
      if (!writer.writeDouble((*_iter1))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}

bool moveGrounding_computeNextMove::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.read(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void moveGrounding_computeNextMove::init(const std::vector<double> & objLocation) {
  this->objLocation = objLocation;
}

moveGrounding::moveGrounding() {
  yarp().setOwner(*this);
}
bool moveGrounding::init(const std::vector<double> & boardLocation) {
  bool _return = false;
  moveGrounding_init helper;
  helper.init(boardLocation);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool moveGrounding::init(const std::vector<double> & boardLocation)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
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
yarp::sig::Vector moveGrounding::computeNextMove(const std::vector<double> & objLocation) {
  yarp::sig::Vector _return;
  moveGrounding_computeNextMove helper;
  helper.init(objLocation);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","yarp::sig::Vector moveGrounding::computeNextMove(const std::vector<double> & objLocation)");
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
    if (tag == "init") {
      std::vector<double>  boardLocation;
      {
        boardLocation.clear();
        uint32_t _size2;
        yarp::os::idl::WireState _etype5;
        reader.readListBegin(_etype5, _size2);
        boardLocation.resize(_size2);
        uint32_t _i6;
        for (_i6 = 0; _i6 < _size2; ++_i6)
        {
          if (!reader.readDouble(boardLocation[_i6])) {
            reader.fail();
            return false;
          }
        }
        reader.readListEnd();
      }
      bool _return;
      _return = init(boardLocation);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
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
      std::vector<double>  objLocation;
      {
        objLocation.clear();
        uint32_t _size7;
        yarp::os::idl::WireState _etype10;
        reader.readListBegin(_etype10, _size7);
        objLocation.resize(_size7);
        uint32_t _i11;
        for (_i11 = 0; _i11 < _size7; ++_i11)
        {
          if (!reader.readDouble(objLocation[_i11])) {
            reader.fail();
            return false;
          }
        }
        reader.readListEnd();
      }
      yarp::sig::Vector _return;
      _return = computeNextMove(objLocation);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.write(_return)) return false;
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
    helpString.push_back("init");
    helpString.push_back("reset");
    helpString.push_back("computeNextMove");
    helpString.push_back("help");
  }
  else {
    if (functionName=="init") {
      helpString.push_back("bool init(const std::vector<double> & boardLocation) ");
    }
    if (functionName=="reset") {
      helpString.push_back("bool reset() ");
    }
    if (functionName=="computeNextMove") {
      helpString.push_back("yarp::sig::Vector computeNextMove(const std::vector<double> & objLocation) ");
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


