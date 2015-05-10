#pragma once

#include "Common.h"

class Node {
public:

  enum Type {
    SERVER,
    CLIENT
  };
  
  typedef int Rank;
  
  Node() : Node(CLIENT) {}
  Node(Type type) : Node(type, 1) {}
  Node(Type type, Rank rank) : type_(type), rank_(rank), num_processes_(0) {}
  
  void set_num_processes(int num_processes) { num_processes_ = num_processes; }
  
  virtual void Run() = 0;
  
protected:
  Type type_;
  Rank rank_;
  int num_processes_;
  Matrix matrix_;
  int k_limit_;
  int delta_k_;
  int k_min_;
  int k_max_;
  
  void Init() {
    k_limit_ = std::min(matrix_.getNumLines(), matrix_.getNumColumns());
    delta_k_ = ceil(k_limit_, num_processes_);
    k_min_ = std::min(delta_k_ * rank_, k_limit_);
    k_max_ = std::min(delta_k_ * (rank_ + 1), k_limit_);
  }
  
  int MaxPivot(int k, float &val){
    int i_max = -1;
    float abs_max = 0.0f;
    
    //printf("rank %d k_min %d k_max %d k %d\n", rank_, k_min_, k_max_, k);
    
    for(int i = std::max(k_min_, k); i < k_max_; ++i){
      if(std::abs(matrix_[i][k]) > abs_max) {
        i_max = i;
        abs_max = std::abs(matrix_[i][k]);
      }
    }
    
    val = abs_max;
    return i_max;
  }
  
  int RankOfOwner(int k){
    int owner;
    
    if(k == k_limit_)
      owner = num_processes_ - 1;
    else {
      owner = k / delta_k_;
    }
    
    return owner;
  }
  
  void SwapRows(int dst, int src){
    int num_columns = matrix_.getNumColumns();
    int dst_rank = RankOfOwner(dst);
    int src_rank = RankOfOwner(src);
    
    //printf("Swap %d %d\n", dst_rank, src_rank);
    
    if(dst_rank == rank_ && src_rank == rank_) {
      float *tmp = new float[num_columns];
      float *line_dst = matrix_[dst];
      float *line_src = matrix_[src];
      
      memcpy(tmp, line_dst, num_columns * sizeof(line_dst[0]));
      memcpy(line_dst, line_src, num_columns * sizeof(line_dst[0]));
      memcpy(line_src, tmp, num_columns * sizeof(line_dst[0]));
      
      delete[]tmp;
    } else if(dst_rank == rank_) {
      float *line_dst = matrix_[dst];
      float *line_src = new float[num_columns];
      
      MPI::COMM_WORLD.Recv(line_src, num_columns, MPI_FLOAT, src_rank, 0);
      MPI::COMM_WORLD.Send(line_dst, num_columns, MPI_FLOAT, src_rank, 0);
      
      memcpy(line_dst, line_src, num_columns * sizeof(line_dst[0]));
      
      delete[]line_src;
    } else if(src_rank == rank_) {
      float *line_src = matrix_[src];
      
      MPI::COMM_WORLD.Send(line_src, num_columns, MPI_FLOAT, dst_rank, 0);
      MPI::COMM_WORLD.Recv(line_src, num_columns, MPI_FLOAT, dst_rank, 0);
    }
  }
  
  void RequestPivot(int k) {
    int num_columns = matrix_.getNumColumns();
    int pivot_rank = RankOfOwner(k);
    
    if(pivot_rank == rank_){
      float a_kk = matrix_[k][k];
      
      if(abs(a_kk) < 1e-6)
        matrix_[k][k] = 0.0f;
      else
        for(int j = k; j < num_columns; ++j)
          matrix_[k][j] /= a_kk;
    
      MPI::COMM_WORLD.Bcast(matrix_[k], num_columns, MPI_FLOAT, pivot_rank);
    } else{
      float *my_line = matrix_[k];
      
      MPI::COMM_WORLD.Bcast(my_line, num_columns, MPI_FLOAT, pivot_rank);
    }
  }
  
  void Compute(int k) {
    int num_columns = matrix_.getNumColumns();
    
    for(int i = k_min_; i < k_max_; ++i){
      if(i != k){
        for(int j = k + 1; j < num_columns; ++j){
          matrix_[i][j] = matrix_[i][j] - matrix_[k][j] * (matrix_[i][k] / matrix_[k][k]);
        }
        matrix_[i][k] = 0.0f;
      }
    }
  }
  
  void Join(){
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
  
#if(DEBUG == true)
#define JoinDebug() Join(); matrix_.print();
#else
#define JoinDebug() {}
#endif

};
