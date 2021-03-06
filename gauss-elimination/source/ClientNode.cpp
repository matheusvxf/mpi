#include "ClientNode.h"

bool ClientNode::ReceiveBroadcastInput(){
  Matrix m;
  uint8_t *pointer;
  int bytes;
  
  MPI_Bcast(&bytes, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  if(bytes == 0)
  	return false;
  
  pointer = new uint8_t[bytes];
  
  MPI_Bcast(pointer, bytes, MPI_BYTE, 0, MPI_COMM_WORLD);
  m.Desserialize(pointer, bytes);
  
  matrix_ = m;
  return true;
}

void ClientNode::GaussElimination(){
      
  for(int k = 0; k < k_limit_; ++k){
  	int pivo = Tournment(k);
  	
  	SwapRows(k, pivo);
    RequestPivot(k);
    
    if(matrix_[k][k] == 0.0f)
      break;
    
    Compute(k);
    JoinDebug();
  }
  
  Join();
}

void ClientNode::Run(){
	while(ReceiveBroadcastInput()) {
  	Init();

#if(DEBUG == true)
  fprintf(stderr, "Client %d k_min %d k_max %d\n", rank_, k_min_, k_max_);
#endif
  	  
  	GaussElimination();
  }
}
