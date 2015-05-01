#include <stdio.h>
#include <mpi.h>
#include <cmath>
#include <algorithm>

static void run_server(int num_procs);
static void run_client(int rank, int num_procs);

int main(int argc, char *argv[]) {
  int rank, namelen, num_procs;
  char processor_name[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  switch(rank){
    case 0:
      run_server(num_procs - 1);
      break;
    default:
      run_client(rank, num_procs - 1);
      break;
  }

  MPI_Finalize();
}

void run_server(int num_procs){
  int n, max, ret;
  
  ret = scanf("%d", &n);
  max = sqrt(n);
  
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  //MPI_Reduce(&my_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  
  //printf("Result %d\n", sum);
}

void run_client(int rank, int num_procs){
  int n, interval, my_sum = 0, sum = 0;
  int *list_s;
  
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  list_s = new int[n];
  
  std::fill(list_s, list_s + n, 0);
  
  
  //MPI_Reduce(&my_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
}
