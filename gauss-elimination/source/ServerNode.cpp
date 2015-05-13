#include "ServerNode.h"

bool ServerNode::ReadInput(){
  int n, m;
  
  std::cin >> n >> m;
  
  if(n == 0 || m == 0)  
  	return false;
  	
  Matrix matrix(n, m);
  
  for(int i = 0; i < n; ++i){
    for(int j = 0; j < m; ++j){
      float in;
      
      std::cin >> in;
      matrix[i][j] = in;
    }
  }
  
  matrix_ = matrix;
  return true;
}

void ServerNode::BroadcastInput(){
  uint8_t *pointer;
  int bytes;
  
  bytes = matrix_.Serialize(pointer);
  
  MPI::COMM_WORLD.Bcast(&bytes, 1, MPI_INT, rank_);
  MPI::COMM_WORLD.Bcast(pointer, bytes, MPI_BYTE, rank_);
}

void ServerNode::GaussElimination(){
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
  matrix_.print("Result:");
}

void ServerNode::Run(){
	int buffer = 0;
#if(DEBUG == true)
	printf("Server %d k_min %d k_max %d\n", rank_, k_min_, k_max_);
#endif

  while(ReadInput()){
  	Matrix solution_2 = matrix_;
  	matrix_.print("---------------------\nInput:");
    BroadcastInput();
		Init();
		
		GaussElimination();
		Test::GaussElimination(solution_2);
		if(matrix_ == solution_2)
			printf("Equal\n");
  }
  
  MPI::COMM_WORLD.Bcast(&buffer, 1, MPI_INT, rank_); // Send message to clients to end execution
}
