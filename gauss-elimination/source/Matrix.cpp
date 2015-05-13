#include "Matrix.h"

// Private functions for testing
static int Tournment(Matrix& m);

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
  printf("%s\n%s\n", c, toString().c_str());
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

int Tournment(Matrix& m){
  int line = 0;
  float max = std::abs(m[0][0]);
  
  for(int i = 1; i < m.getNumLines(); ++i){
    if(std::abs(m[i][0]) > max) {
      max = std::abs(m[i][0]);
      line = i;
    }
  }
  
  return line;
}
  
void Test::GaussElimination(Matrix &m){
  int num_lines = m.getNumLines();
  int num_columns = m.getNumColumns();
  int k_limit = std::min(num_lines, num_columns);
  int pivo = Tournment(m);
  
  if(m[pivo][0] == 0.0f) {
    printf("Matrix is singular\n");
    return;
  }    
  
  for(int j = 0; j < num_columns; ++j){
    m[pivo][j] /= m[pivo][0];
  }
  
  for(int k = 1; k < k_limit; ++k){
  
    if(std::abs(m[k][k]) < 1e-6) {
      printf("Matrix is singular!\n");
      return;
    }
    
    for(int j = k; j < num_columns; ++j)
      m[k][j] /= m[k][k];
     
    for(int i = 0; i < num_lines; ++i){
      if(i != k) {
        for(int j = k; j < num_columns; ++j){
          m[i][j] = m[i][j] - m[k][j] * m[i][k] / m[k][k];
        }
      }
    }
  }
}
