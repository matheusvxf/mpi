#include "TestFramework.h"
#include "Common.h"

#include <fstream>

// Private functions for testing
static int Tournment(Matrix& m, int k = 0);

int Tournment(Matrix& m, int k){
  int line = k;
  float max = std::abs(m[k][k]);
  
  for(int i = k + 1; i < m.getNumLines(); ++i){
    if(std::abs(m[i][k]) > max) {
      max = std::abs(m[i][k]);
      line = i;
    }
  }
  
  return line;
}

void Test::GenerateTest(int num_test){
	std::fstream fs;
	fs.open("test_2.txt", std::fstream::out);

	printf("Generate %d test(s)\n", num_test);
	
	while(num_test--){
		int n = rand() % 10 + 1, m = n + rand() % 10 + 1;
		
		fs << n << " " << m << std::endl;
		for(int i = 0; i < n; ++i){
			for(int j = 0; j < m; ++j){
				fs << rand() % 100 << " ";
			}
			fs << std::endl;
		}
	}
	
	fs << "0 0" << std::endl;
	fs.close();
}
  
void Test::GaussElimination(Matrix &m){
  int num_lines = m.getNumLines();
  int num_columns = m.getNumColumns();
  int k_limit = std::min(num_lines, num_columns);   
  
  for(int k = 0; k < k_limit; ++k){
	  int pivo = Tournment(m, k);
  
		if(std::abs(m[pivo][k]) < PRECISION) {
			m[pivo][k] = 0.0f; // Conformance with the distributed algorithm
		  printf("Matrix is singular!\n");
		  return;
		}
		
		if(pivo != k)
			m.SwapLines(pivo, k);
    
    for(int j = num_columns - 1; j >= k; --j){
      m[k][j] = m[k][j] / m[k][k];
    }
     
    for(int i = 0; i < num_lines; ++i){
      if(i != k) {
        for(int j = k + 1; j < num_columns; ++j){
          m[i][j] = m[i][j] - m[k][j] * m[i][k];
        }
        m[i][k] = 0.0f;
      }
    }
  }
}
