#include "Matrix.h"

#include <sstream>
#include <iostream>

Matrix::Matrix(const Matrix* matrix){
	n_ = matrix->n_;
	m_ = matrix->m_;
	matrix_ = new float[n_ * m_];
	
	for(int i = 0; i < n_; ++i)
	for(int j = 0; j < m_; ++j)
		matrix_[i * m_ + j] = matrix->matrix_[i * m_ + j];	
}

std::string Matrix::Line::toString() const{
	int columns = owner_->getNumColumns();
	std::stringstream buffer;
	
	for(int i = 0; i < columns; ++i)
		buffer << line_[i] << " ";
	
	return buffer.str();
}

void Matrix::Line::print(const char *c) const{
	printf("%s\n%s\n", c, toString().c_str());
}

std::string Matrix::toString(){
  std::stringstream buffer;
  
  for(int i = 0; i < n_; ++i){
    for(int j = 0; j < m_ - 1; ++j){
      buffer << matrix_[i * m_ + j] << " ";
    }
    buffer << matrix_[i * m_ + m_ - 1] << std::endl;
  }
  
  return buffer.str();
}

void Matrix::print(const char *c) {
 	std::cerr << c << std::endl;
 	
 	for(int i = 0; i < n_; ++i){
		for(int j = 0; j < m_ - 1; ++j)
			std::cerr << matrix_[i * m_ + j] << " ";
		std::cerr << matrix_[i * m_ + m_ - 1] << std::endl;
	}
}

bool Matrix::operator==(const Matrix& m) const {
	bool flag = true;
	
	if(getNumLines() != m.getNumLines() || getNumColumns() != m.getNumColumns())
		return false;
	
	for(int i = 0; flag && i < n_; ++i)
	for(int j = 0; flag && j < m_; ++j)
		flag &= (*this)[i][j] == m[i][j];
		
	return flag;
}

Matrix Matrix::clone(){
	return Matrix(this);
}

void Matrix::SwapLines(int line_1, int line_2){
	float *tmp = new float[m_];
	int num_bytes = sizeof(tmp[0]) * m_;
	
	memcpy(tmp, getLine(line_1), num_bytes);
	memcpy(getLine(line_1), getLine(line_2), num_bytes);
	memcpy(getLine(line_2), tmp, num_bytes);
	
	delete[]tmp;
}

int Matrix::Serialize(uint8_t *&pointer){
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

void Matrix::Desserialize(uint8_t *pointer, int bytes){
  if(matrix_ != NULL)
    delete[]matrix_;
  
  n_ = *(int*)pointer;
  m_ = *(int*)(pointer + sizeof(n_));
  matrix_ = new float[n_ * m_];
  
  memcpy(matrix_, pointer + sizeof(n_) + sizeof(m_), n_ * m_ * sizeof(*matrix_));
  delete[]pointer;
}  

