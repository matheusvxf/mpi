#include <stdio.h>
#include <cstring>
#include <mpi.h>
#include <cmath>
#include <algorithm>

static void run_server(int num_procs);
static void run_client(int rank, int num_procs);

enum MSG_TYPE {
  PRIME = 0,
  IGNORE
};

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

inline int ceil(int n, int d) {
  return n / d + (n % d == 0 ? 0 : 1);
}

void run_server(int num_procs){
  int n, ret, max;
  int *proc_index = new int[num_procs];
  int my_accumulator = 0, accumulator = 0;
  
  ret = scanf("%d", &n);
  
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  std::fill(proc_index, proc_index + num_procs, 0);
  
  while(1){
    max = 0;
    for(int i = 1; i < num_procs; ++i)
    {
      int val;
      
      MPI::COMM_WORLD.Recv(&val, 1, MPI::INT, i, PRIME);
      max = std::max(max, val); 
    }
    
    if(max != 0){
      MPI::COMM_WORLD.Bcast(&max, 1, MPI_INT, 0);
      
      //printf("Prime: %d\n", max);
    } else {
      int tmp = 0;
      MPI::COMM_WORLD.Bcast(&tmp, 1, MPI_INT, 0);
      break;
    }
  }
  
  MPI_Reduce(&my_accumulator, &accumulator, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  
  printf("# Primes smaller than %d = %d\n", n, accumulator);
}

void run_client(int rank, int num_procs){
  int n, interval, index, upper_bound, lower_bound, share, limit, i;
  int prime, my_accumulator = 0, accumulator = 0;
  bool *prime_s;
  
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  share = ceil(n, num_procs);
  prime_s = new bool[share];
  std::fill(prime_s, prime_s + share, true);
  
  limit = sqrt(n);
  lower_bound = share * (rank - 1) + 1;
  lower_bound = (lower_bound > 1 ? lower_bound : 2);
  upper_bound = share * rank;
  upper_bound = (upper_bound <= n ? upper_bound : n); 
  index = lower_bound;
  i = 0;
  
  printf("Process %d: [%d, %d]\n", rank, lower_bound, upper_bound);
  
  while(index <= upper_bound){
    while(index < limit && i < share && prime_s[i] == false) {
      i++;
      index++;
    }
    
    if(i < share && index < limit)
      MPI::COMM_WORLD.Send(&index, 1, MPI::INT, 0, PRIME);
    else {
      int tmp = 0;
      MPI::COMM_WORLD.Send(&tmp, 1, MPI::INT, 0, PRIME);
    }
    
    MPI::COMM_WORLD.Bcast(&prime, 1, MPI_INT, 0);
    
    if(prime == 0){
      break;
    } else if(prime == index){
      i++;
      index++;
    }
    
    for(int j = i; j < share; j++){
      if(((j + lower_bound) % prime) == 0)
        prime_s[j] = false;
    }
  }
  
  for(int i = 0, index = lower_bound; i <= share && index <= upper_bound; ++i, ++index){
    if(prime_s[i] == true) {
      //printf("%d\n", index);
      my_accumulator++;
    }
  }
  
  MPI_Reduce(&my_accumulator, &accumulator, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
}
