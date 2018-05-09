
#include <mpi.h>
#include <iostream>
#include <unistd.h>

#include "solve_data.h"

int main(int argc, char* argv[]) {
  int size, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  SolveData data(size, rank);
  bool next = true;

  size_t m = 0;
  while (next) {
    if (m++ % 10000 == 0 && !rank) {
      data.dumpIteration();
      printf("%lu. #############################\n", m - 1);
    }
    data.calculateConcurrentBorders();
    data.sendBorders();
    data.calculateCenter();
    data.waitCommunication();
    next = data.needNext();
    data.prepareNext();
  }

  MPI_Finalize();
  return 0;
}

