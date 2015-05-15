#include "Node.h"

void Node::Init() {
  k_limit_ = std::min(matrix_.getNumLines(), matrix_.getNumColumns());
  delta_k_ = ceil(k_limit_, num_processes_);
  k_min_ = std::min(delta_k_ * rank_, k_limit_);
  k_max_ = std::min(delta_k_ * (rank_ + 1), k_limit_);
}

int Node::Tournment(int k) {
	TournmentReduce reduce, result;
	int &i_max = reduce.rank;
	float &abs_max = reduce.pivo;
	
	i_max = -1;
	abs_max = -1.0f;
	for(int i = std::max(k_min_, k); i < std::max(k_max_, k); ++i){
		if(std::abs(matrix_[i][k]) > abs_max){
			i_max = i;
			abs_max = std::abs(matrix_[i][k]);
		}
	}
	
	MPI::COMM_WORLD.Reduce(&reduce, &result, 1, MPI_FLOAT_INT, MPI_MAXLOC, ROOT);
  MPI::COMM_WORLD.Bcast(&result, 1, MPI_FLOAT_INT, ROOT);
  
  return result.rank;
}

int Node::RankOfOwner(int k){
  int owner;
  
  if(k == k_limit_)
    owner = num_processes_ - 1;
  else {
    owner = k / delta_k_;
  }
  
  return owner;
}

void Node::SwapRows(int dst, int src){
  int num_columns = matrix_.getNumColumns();
  int dst_rank = RankOfOwner(dst);
  int src_rank = RankOfOwner(src);
  int num_bytes = num_columns * sizeof(matrix_[0][0]);
  
  if(dst == src)
  	return;
  
  if(dst_rank == rank_ && src_rank == rank_) {
    matrix_.SwapLines(src, dst);
    
  } else if(dst_rank == rank_) {
    float *line_dst = matrix_[dst];
    float *line_src = new float[num_columns];
    
    MPI::COMM_WORLD.Recv(line_src, num_columns, MPI_FLOAT, src_rank, 0);
    MPI::COMM_WORLD.Send(line_dst, num_columns, MPI_FLOAT, src_rank, 0);
    
    memcpy(line_dst, line_src, num_bytes);
    
    delete[]line_src;
  } else if(src_rank == rank_) {
    float *line_src = matrix_[src];
    
    MPI::COMM_WORLD.Send(line_src, num_columns, MPI_FLOAT, dst_rank, 0);
    MPI::COMM_WORLD.Recv(line_src, num_columns, MPI_FLOAT, dst_rank, 0);
  }
}

void Node::RequestPivot(int k) {
  int num_columns = matrix_.getNumColumns();
  int pivot_rank = RankOfOwner(k);
  
  if(pivot_rank == rank_){
    
    if(std::abs(matrix_[k][k]) < PRECISION){
      matrix_[k][k] = 0.0f; // To signal singular matrix and stop
      MPI::COMM_WORLD.Bcast(matrix_[k], num_columns, MPI_FLOAT, pivot_rank);
    } else {
      for(int j = num_columns - 1; j >= k; --j) // Back forward to change m[k][k] at end
        matrix_[k][j] /= matrix_[k][k];
      
	    MPI::COMM_WORLD.Bcast(matrix_[k], num_columns, MPI_FLOAT, pivot_rank);
  	}
  		
  } else{
    float *my_line = matrix_[k];
    
    MPI::COMM_WORLD.Bcast(my_line, num_columns, MPI_FLOAT, pivot_rank);
  }
}

void Node::Compute(int k) {
  int num_columns = matrix_.getNumColumns();
  
  for(int i = k_min_; i < k_max_; ++i){
    if(i != k){
      for(int j = k + 1; j < num_columns; ++j){
        matrix_[i][j] = matrix_[i][j] - matrix_[k][j] * matrix_[i][k];
      }
      matrix_[i][k] = 0.0f;
    }
  }
}

void Node::Join(){
  int num_columns = matrix_.getNumColumns();
  
  if(type_ == SERVER){
    for(int i = 1; i < num_processes_; ++i){
      int k_min = std::min(delta_k_ * i, k_limit_);
      int k_max = std::min(delta_k_ * (i + 1), k_limit_);
      
      for(int j = k_min; j < k_max; ++j)
        MPI::COMM_WORLD.Recv(matrix_[j], num_columns, MPI_FLOAT, i, 0);
    }
  } else {
    for(int j = k_min_; j < k_max_; ++j)
      MPI::COMM_WORLD.Send(matrix_[j], num_columns, MPI_FLOAT, 0, 0);
  }
}

