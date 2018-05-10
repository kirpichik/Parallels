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

  if (rank == 0) {
    data.dumpIteration();
    std::cout << "0. #################################\n" << std::endl;
  }

  size_t iter = 0;
  while (next) {
    data.calculateConcurrentBorders();
    data.sendBorders();
    data.calculateCenter();
    data.waitCommunication();
    next = !data.needNext();
    if (iter++ % 10000 == 0 && rank == 0) {
      data.dumpIteration();
      std::cout << "Iter number(in 10k) "<< iter << ". #############################" << std::endl;
    }
    data.prepareNext();
  }

  MPI_Finalize();
  return (EXIT_SUCCESS);
}