
#include <mpi.h>
#include <iostream>
#include <string>

#include "solve.h"

int main(int argc, char* argv[]) {
  int size, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  SolveData data(size, rank);
  bool next = true;

  while (next) {
    data.calculateConcurrentBorders();
    data.sendBorders();
    data.calculateCenter();
    next = data.needNext();
    data.prepareNextStep();
  }


  MPI_Finalize();
  return 0;
}

