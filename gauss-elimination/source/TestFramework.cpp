#include "TestFramework.h"
#include "Common.h"

#include <fstream>

// Private functions for testing
static int Tournment(Matrix& m, int k = 0);

int Tournment(Matrix& m, int k){
  int line = 0;
  float max = std::abs(m[k][k]);
  
  for(int i = k + 1; i < m.getNumLines(); ++i){
    if(std::abs(m[i][k]) > max) {
      max = std::abs(m[i][k]);
      line = i;
    }
  }
  
  return line;
}

void SwapLine(Matrix& m, int src, int dst){
	Matrix::Line tmp = m[src];
	
	printf("%d %d\n", src, dst);
	m.print("before\n");
	m[src] = m[dst];
	m[dst] = tmp;
	m.print("after\n");
}

void Test::GenerateTest(int num_test){
	std::fstream fs;
	fs.open("test_2.txt", std::fstream::out);

	printf("Generate %d test(s)\n", num_test);
	
	while(num_test--){
		int n = rand() % 10 + 1, m = rand() % 10 + 1;
		
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
		  printf("Matrix is singular!\n");
		  return;
		}
		
		printf("Pivo %d\n", pivo);
		m.print();
		m.SwapLines(pivo, k);
		m.print();
    
    for(int j = k; j < num_columns; ++j){
      m[k][j] /= m[k][k];
    }
     
    for(int i = 0; i < num_lines; ++i){
      if(i != k) {
        for(int j = k; j < num_columns; ++j){
          m[i][j] = m[i][j] - m[k][j] * m[i][k] / m[k][k];
        }
      }
    }
  }
}
