#include <stdio.h>
#include <mpi.h>

static void run_server();
static void run_client(int rank, int num_procs);

int main(int argc, char *argv[]) {
  int rank, namelen, num_procs;
  char processor_name[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  switch(rank){
    case 0:
      run_server();
      break;
    default:
      run_client(rank, num_procs - 1);
      break;
  }

  MPI_Finalize();
}

void run_server(){
  int num_procs, n, my_sum = 0, sum = 0, ret;
  
  ret = scanf("%d", &n);
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Reduce(&my_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  
  printf("Result %d\n", sum);
}

void run_client(int rank, int num_procs){
  int n_max, interval, my_sum = 0, sum = 0;
  
  MPI_Bcast(&n_max, 1, MPI_INT, 0, MPI_COMM_WORLD);
  interval = (n_max / num_procs) + (n_max % num_procs >= 1 ? 1 : 0); 
  
  for(int i = interval * (rank - 1); i <= n_max && i < interval * rank; ++i) {
    my_sum += i;
  }
  
  MPI_Reduce(&my_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
}
