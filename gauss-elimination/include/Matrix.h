#pragma once

#include <algorithm>
#include <cstring>
#include <string>
#include <sstream>

class Matrix {
  public:
  
  int n_, m_;
  float *matrix_;
  
  Matrix() : n_(0), m_(0), matrix_(NULL) {}
  
  Matrix(int n, int m) : n_(n), m_(m), matrix_(NULL) {
    matrix_ = new float[n_ * m_];
    
    std::fill(matrix_, matrix_ + n_ * m_, 0);
  }
  
  inline const float *operator[](int line) const {
  	return matrix_ + line * m_;
  }
  
  inline float *operator[](int line){
    return matrix_ + line * m_;
  }
  
  bool operator==(const Matrix& m) const;
  inline int getNumLines() const { return n_; }
  inline int getNumColumns() const { return m_; }  
  
  std::string toString();
  void print(const char *c);
  int Serialize(uint8_t *&pointer);
  void Desserialize(uint8_t *pointer, int bytes);  
};

// Public functions for testing
namespace Test {
	extern void GaussElimination(Matrix &m);
}

