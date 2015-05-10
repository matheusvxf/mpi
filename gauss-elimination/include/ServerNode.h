#pragma once

#include <iostream>

#include "Node.h"

class ServerNode : public Node {
  
  Matrix ReadInput(){
    int n, m;
    
    std::cin >> n >> m;
    Matrix matrix(n, m);
    
    for(int i = 0; i < n; ++i){
      for(int j = 0; j < m; ++j){
        float in;
        
        std::cin >> in;
        matrix[i][j] = in;
      }
    }
    
    return matrix;
  }

  void BroadcastInput(){
    uint8_t *pointer;
    int bytes;
    
    bytes = matrix_.Serialize(pointer);
    
    MPI_Bcast(&bytes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pointer, bytes, MPI_BYTE, 0, MPI_COMM_WORLD);
  }

  int Tournment(){
    float val_max;
    int row_max = MaxPivot(0, val_max);
    
    for(int i = 1; i < num_processes_; ++i){
      int row;
      float val;
      
      MPI::COMM_WORLD.Recv(&row, 1, MPI::INT, i, 0);
      MPI::COMM_WORLD.Recv(&val, 1, MPI::FLOAT, i, 0);
      
      if(val > val_max) {
        val_max = val;
        row_max = row;
      }
    }
    
    MPI::COMM_WORLD.Bcast(&row_max, 1, MPI::INT, 0);
    return row_max;
  }
  
  void GaussElimination(){
    int i_max = Tournment();
    
    if(i_max == -1) {
      printf("Matrix is singular!\n");
      return;
    }
     
    SwapRows(0, i_max);
    
    for(int k = 0; k < k_limit_; ++k){
#if(DEBUG == true)
      printf("step %d\n", k);
#endif      
      RequestPivot(k);
      if(matrix_[k][k] == 0.0f){
        printf("Matrix is singular!\n");
        break;
      }
      
      Compute(k);
      JoinDebug();
    }
    
    Join();
    printf("Result:\n");
    matrix_.print();
  }

public:

  ServerNode(Rank rank) : Node(SERVER, rank) {}
  
  void Run(){
    matrix_ = ReadInput();
    BroadcastInput();
    Init();
    
#if(DEBUG == true)
    printf("Server %d k_min %d k_max %d\n", rank_, k_min_, k_max_);
#endif
    
    GaussElimination();
  }
};
