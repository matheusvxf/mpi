#pragma once

#include <algorithm>
#include <cstring>
#include <string>
#include <sstream>

class Matrix {
  public:
  
  class Line {
		float *line_;
		const Matrix *owner_;
		
		public:
		Line(float *line, const Matrix *owner) : line_(line), owner_(owner) {}
		
	  inline const float& operator[](int column) const {
			return line_[column];
		}
		
		inline float& operator[](int column){
		  return line_[column];
		}
		
		inline float *line() { return line_; }
		
		Line &operator=(const Line& l){
			int columns = owner_->getNumColumns();
			float *tmp = new float[columns];
			int bytes = columns * sizeof(tmp[0]);
			
			
			printf("%s\n", toString().c_str());
			
			memcpy(tmp, line_, bytes);
			memcpy(line_, l.line_, bytes);
			memcpy(l.line_, tmp, bytes);
			
			printf("%s\n", toString().c_str());
			
			delete[]tmp;
			return (*this);
		}
		
		std::string toString() const;
		void print(const char *c = "") const;
	};
  
  friend class Line;
  
  int n_, m_;
  float *matrix_;
  
  Matrix() : n_(0), m_(0), matrix_(NULL) {}
  Matrix(const Matrix* matrix);

  Matrix(int n, int m) : n_(n), m_(m), matrix_(NULL) {
    matrix_ = new float[n_ * m_];
    
    std::fill(matrix_, matrix_ + n_ * m_, 0);
  }
  
  /*inline const float* operator[](int line) const{
  	return matrix_ + line * m_;
  }*/
  
  inline float* operator[](int line){
    return matrix_ + line * m_;
  }
		
	void SwapLines(int line_1, int line_2);
  
  bool operator==(const Matrix& m) const;
  inline int getNumLines() const { return n_; }
  inline int getNumColumns() const { return m_; }
  inline float* getLine(int line) const { return matrix_ + line * m_; }
  
  Matrix clone();
  
  std::string toString();
  void print(const char *c = "");
  int Serialize(uint8_t *&pointer);
  void Desserialize(uint8_t *pointer, int bytes);  
};

