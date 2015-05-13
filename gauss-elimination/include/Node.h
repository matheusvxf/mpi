#pragma once

#include <algorithm>

#include "mpi.h"
#include "Common.h"
#include "Matrix.h"

#define ROOT 0

class Node {
private:
	struct TournmentReduce {
		float pivo;
		int rank;
	};

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
  
  void Init();
  int Tournment();
  int RankOfOwner(int k);
  void SwapRows(int dst, int src);
  void RequestPivot(int k);
  void Compute(int k);
  void Join();
  
#if(DEBUG == true)
#define JoinDebug() Join(); matrix_.print();
#else
#define JoinDebug() {}
#endif

};
