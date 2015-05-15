#pragma once

#include <iostream>

#include "Node.h"

class ServerNode : public Node {
  
  bool ReadInput();
  void BroadcastInput();
  void GaussElimination();

public:

  ServerNode(Rank rank) : Node(SERVER, rank) {}
  
  void Run();
  void TestRun();
  void NormalRun();
 
#undef JoinDebug
#if(DEBUG == true)
#define JoinDebug(x) Join(); matrix_.print(x);
#else
#define JoinDebug(x) {}
#endif
};
