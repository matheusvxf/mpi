#pragma once

#include "Node.h"
#include "Matrix.h"

class ClientNode : public Node {
	
	bool ReceiveBroadcastInput();
	void GaussElimination();

public:
  
  ClientNode(Rank rank) : Node(CLIENT, rank) {}

	void Run();  
	
#undef JoinDebug
#if(DEBUG == true)
#define JoinDebug() Join();
#else
#define JoinDebug() {}
#endif
};
