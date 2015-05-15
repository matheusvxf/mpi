#include "ServerNode.h"
#include "TestFramework.h"

bool ServerNode::ReadInput(){
  int n, m;
  
  std::cin >> n >> m;
  
  if(n == 0 || m == 0)  
  	return false;
  
  if(n > m) {
  	fprintf(stderr, "The system is inconsistent\n");
  	return false;
  }	
  
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
  for(int k = 0; k < k_limit_; ++k){
	  int pivo = Tournment(k);
	  
	  SwapRows(k, pivo);
    RequestPivot(k);
    
    if(matrix_[k][k] == 0.0f){
      fprintf(stderr, "Matrix is singular!\n");
      break;
    }
    
    Compute(k);
    JoinDebug("ServerNode::GaussElimination");
  }
  
  Join();
}

void ServerNode::NormalRun(){
	while(ReadInput()){		
		Matrix solution_2 = matrix_.clone();
		matrix_.print("---------------------\nInput:");
		BroadcastInput();
		Init();

	#if(DEBUG == true)
	fprintf(stderr, "Server %d k_min %d k_max %d\n", rank_, k_min_, k_max_);
	#endif
	
		GaussElimination();
		Test::GaussElimination(solution_2);
	 	matrix_.print("Parallel Result:");
	 	
	 	if(matrix_ != solution_2){
	 		solution_2.print("Interative Result:");
 		}
	}
}

void ServerNode::TestRun(){
	int test_count = 0, equal_count = 0;
	
	while(ReadInput()){
  	Matrix solution_2 = matrix_.clone();
    BroadcastInput();
		Init();
		test_count++;

#if(DEBUG == true)
	fprintf(stderr, "Server %d k_min %d k_max %d\n", rank_, k_min_, k_max_);
#endif
		
		GaussElimination();
		Test::GaussElimination(solution_2);
		if(matrix_ == solution_2)
			equal_count++;
  }
  
  fprintf(stderr, "Right %d/%d\n", equal_count, test_count);
}

void ServerNode::Run(){
	int buffer = 0;
	
	printf("Server Running\n");
	
	TestRun();
  
  MPI::COMM_WORLD.Bcast(&buffer, 1, MPI_INT, rank_); // Send message to clients to end execution
}
