#include "Matrix.h"

// Private functions for testing
static int Tournment(Matrix& m);

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
  
void GaussElimination(Matrix &m){
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
      printf("Matrix is singular\n");
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
