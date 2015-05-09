mpic++ code.cpp -o code -Wall -std=c++11
mpirun -np 3 ./code < test.txt
