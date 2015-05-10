#pragma once

#include "Node.h"
#include "Matrix.h"

class ClientNode : public Node {

  Matrix ReceiveBroadcastInput(){
    Matrix m;
    uint8_t *pointer;
    int bytes;
    
    MPI_Bcast(&bytes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    pointer = new uint8_t[bytes];
    
    MPI_Bcast(pointer, bytes, MPI_BYTE, 0, MPI_COMM_WORLD);
    m.Desserialize(pointer, bytes);
    
    return m;
  }
  
  int Tournment(){
    float val_max;
    int row_max = MaxPivot(0, val_max);

    MPI::COMM_WORLD.Send(&row_max, 1, MPI::INT, 0, 0);
    MPI::COMM_WORLD.Send(&val_max, 1, MPI::FLOAT, 0, 0);
    MPI::COMM_WORLD.Bcast(&row_max, 1, MPI::INT, 0);
    return row_max;
  }
  
  void GaussElimination(){
    int i_max = Tournment();
    
    if(i_max == -1)
        return;
        
    SwapRows(0, i_max);
        
    for(int k = 0; k < k_limit_; ++k){
      RequestPivot(k);
      
      if(matrix_[k][k] == 0.0f)
        break;
      
      Compute(k);
      JoinDebug();
    }
    
    Join();
  }

public:
  
  ClientNode(Rank rank) : Node(CLIENT, rank) {}

  void Run(){
    matrix_ = ReceiveBroadcastInput();
    Init();
    
#if(DEBUG == true)
    printf("Client %d k_min %d k_max %d\n", rank_, k_min_, k_max_);
#endif
    
    GaussElimination();
  }
  
};
