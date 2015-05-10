#pragma once

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
