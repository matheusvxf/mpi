#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <mpi.h>
#include <cmath>
#include <algorithm>

#include "Matrix.h"
#include "ServerNode.h"
#include "ClientNode.h"

int main(int argc, char *argv[]) {
  int rank, num_procs;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  switch(rank){
    case 0: {
      ServerNode server(rank);
      server.set_num_processes(num_procs);
      server.Run();
      break;
    }
    default: {
      ClientNode client(rank);
      client.set_num_processes(num_procs);
      client.Run();
      break;
    }
  }

  MPI_Finalize();
}




