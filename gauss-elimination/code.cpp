#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <mpi.h>
#include <cmath>
#include <algorithm>


typedef unsigned char uint8_t;
#define DEBUG false

int ceil(int n, int d) {
  return n / d + (n % d == 0 ? 0 :1);
}

class Matrix {
  public:
  
  int n_, m_;
  float *matrix_;
  
  Matrix() : n_(0), m_(0), matrix_(NULL) {}
  
  Matrix(int n, int m) : n_(n), m_(m), matrix_(NULL) {
    matrix_ = new float[n_ * m_];
    
    std::fill(matrix_, matrix_ + n_ * m_, 0);
  }
  
  inline float *operator[](int line){
    return matrix_ + line * m_;
  }
  
  inline int getNumLines() { return n_; }
  inline int getNumColumns() { return m_; }  
  
  std::string toString(){
    std::stringstream buffer;
    
    for(int i = 0; i < n_; ++i){
      for(int j = 0; j < m_ - 1; ++j){
        buffer << matrix_[i * m_ + j] << " ";
      }
      buffer << matrix_[i * m_ + m_ - 1] << std::endl;
    }
    
    return buffer.str();
  }
  
  void print() {
    printf("%s\n", toString().c_str());
  }
  
  int Serialize(uint8_t *&pointer){
    int bytes = n_ * m_ * sizeof(*matrix_) + sizeof(n_) + sizeof(m_);
    
    pointer = new uint8_t[bytes];
    int &n = *(int*)pointer;
    int &m = *(int*)(pointer + sizeof(n));
    float *matrix = (float*)(pointer + sizeof(n) + sizeof(m));
    
    n = n_;
    m = m_;
    memcpy(matrix, matrix_, n_ * m_ * sizeof(*matrix_));
    
    return bytes;    
  }
  
  void Desserialize(uint8_t *pointer, int bytes){
    if(matrix_ != NULL)
      delete[]matrix_;
    
    n_ = *(int*)pointer;
    m_ = *(int*)(pointer + sizeof(n_));
    matrix_ = new float[n_ * m_];
    
    memcpy(matrix_, pointer + sizeof(n_) + sizeof(m_), n_ * m_ * sizeof(*matrix_));
    delete[]pointer;
  }
};

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

int main(int argc, char *argv[]) {
  int rank, num_procs;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  switch(rank){
    case 0: {
      ServerNode server(rank);
      server.set_num_processes(num_procs);
      server.Run();
      break;
    }
    default: {
      ClientNode client(rank);
      client.set_num_processes(num_procs);
      client.Run();
      break;
    }
  }

  MPI_Finalize();
}




