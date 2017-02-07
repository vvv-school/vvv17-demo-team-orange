#movePlanner.thrift
service movePlanner_IDL {
  i32 get_answer(1: i32 oppMove);
  bool reset();
}
