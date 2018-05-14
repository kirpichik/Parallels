#include <mpi.h>
#include <iostream>

#include "solve_data.h"

const long double EPSILON_PARAM = 10e-8;
const long double A_PARAM = 10e5;

int main(int argc, char* argv[]) {
  int size, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  SolveData data(size, rank, EPSILON_PARAM, A_PARAM);
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
    data.prepareNext();
    if (rank == 0) {
      data.dumpIteration();
      std::cout << "Iter number " << ++iter << ". #############################"
                << std::endl;
    }
  }

  MPI_Finalize();
  return (EXIT_SUCCESS);
}
