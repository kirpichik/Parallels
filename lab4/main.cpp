
#include <mpi.h>
#include <iostream>

#include "solve_data.h"

int main(int argc, char* argv[]) {
  int size, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  SolveData data(size, rank);
  bool next = true;

  while (next) {
    std::cout << rank << ". Step 1" << std::endl;
    data.calculateConcurrentBorders();
    std::cout << rank << ". Step 2" << std::endl;
    data.sendBorders();
    std::cout << rank << ". Step 3" << std::endl;
    data.calculateCenter();
    std::cout << rank << ". Step 4" << std::endl;
    next = data.needNext();
    std::cout << rank << ". Step 5" << std::endl;
    data.prepareNextStep();
  }

  MPI_Finalize();
  return 0;
}

