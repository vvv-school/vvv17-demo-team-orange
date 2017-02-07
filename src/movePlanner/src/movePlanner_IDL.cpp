// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <movePlanner_IDL.h>
#include <yarp/os/idl/WireTypes.h>



class movePlanner_IDL_computeNextMove : public yarp::os::Portable {
public:
  std::vector<int32_t>  state;
  std::vector<int32_t>  _return;
  void init(const std::vector<int32_t> & state);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class movePlanner_IDL_reset : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool movePlanner_IDL_computeNextMove::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("computeNextMove",1,1)) return false;
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(state.size()))) return false;
    std::vector<int32_t> ::iterator _iter0;
    for (_iter0 = state.begin(); _iter0 != state.end(); ++_iter0)
    {
      if (!writer.writeI32((*_iter0))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}

bool movePlanner_IDL_computeNextMove::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  {
    _return.clear();
    uint32_t _size1;
    yarp::os::idl::WireState _etype4;
    reader.readListBegin(_etype4, _size1);
    _return.resize(_size1);
    uint32_t _i5;
    for (_i5 = 0; _i5 < _size1; ++_i5)
    {
      if (!reader.readI32(_return[_i5])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}

void movePlanner_IDL_computeNextMove::init(const std::vector<int32_t> & state) {
  this->state = state;
}

bool movePlanner_IDL_reset::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("reset",1,1)) return false;
  return true;
}

bool movePlanner_IDL_reset::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void movePlanner_IDL_reset::init() {
  _return = false;
}

movePlanner_IDL::movePlanner_IDL() {
  yarp().setOwner(*this);
}
std::vector<int32_t>  movePlanner_IDL::computeNextMove(const std::vector<int32_t> & state) {
  std::vector<int32_t>  _return;
  movePlanner_IDL_computeNextMove helper;
  helper.init(state);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::vector<int32_t>  movePlanner_IDL::computeNextMove(const std::vector<int32_t> & state)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool movePlanner_IDL::reset() {
  bool _return = false;
  movePlanner_IDL_reset helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool movePlanner_IDL::reset()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool movePlanner_IDL::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "computeNextMove") {
      std::vector<int32_t>  state;
      {
        state.clear();
        uint32_t _size6;
        yarp::os::idl::WireState _etype9;
        reader.readListBegin(_etype9, _size6);
        state.resize(_size6);
        uint32_t _i10;
        for (_i10 = 0; _i10 < _size6; ++_i10)
        {
          if (!reader.readI32(state[_i10])) {
            reader.fail();
            return false;
          }
        }
        reader.readListEnd();
      }
      std::vector<int32_t>  _return;
      _return = computeNextMove(state);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        {
          if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(_return.size()))) return false;
          std::vector<int32_t> ::iterator _iter11;
          for (_iter11 = _return.begin(); _iter11 != _return.end(); ++_iter11)
          {
            if (!writer.writeI32((*_iter11))) return false;
          }
          if (!writer.writeListEnd()) return false;
        }
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

std::vector<std::string> movePlanner_IDL::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("computeNextMove");
    helpString.push_back("reset");
    helpString.push_back("help");
  }
  else {
    if (functionName=="computeNextMove") {
      helpString.push_back("std::vector<int32_t>  computeNextMove(const std::vector<int32_t> & state) ");
    }
    if (functionName=="reset") {
      helpString.push_back("bool reset() ");
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


