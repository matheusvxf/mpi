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
};
