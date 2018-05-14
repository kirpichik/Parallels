#include <mpi.h>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "solve_data.h"

SolveData::SolveData(size_t proc_count, size_t rank) {
  // x0, y0, z0
  center = Point<double>(-1, -1, -1);
  // Dx, Dy, Dz
  distance = Point<double>(2, 2, 2);
  // Nx, Ny, Nz
  grid = Point<size_t>(8, 8, 8);
  // h_x, h_y, h_z
  height.x = distance.x / (grid.x + 1);
  height.y = distance.y / (grid.y + 1);
  height.z = distance.z / (grid.z + 1);

  paramA = 10e5;
  epsilon = 10e-8;
  initial_approx = 0;
  this->rank = rank;
  this->proc_count = proc_count;

  if (grid.x % proc_count != 0)
    throw std::runtime_error("Invalid process count.");

  Point<size_t> size(grid.x / proc_count, grid.y, grid.z);
  currentArea = new Area(size, initial_approx);
  nextArea = new Area(size, initial_approx);
  borderLower = !rank;
  borderUpper = (rank + 1) == proc_count;

  initBorders();
  nextArea->copyData(*currentArea);
}

SolveData::~SolveData() {
  delete currentArea;
  delete nextArea;
}

void SolveData::initBorders() {
  const Point<size_t>& size = currentArea->size;

  // Противоположные плоскости z->x
  for (size_t i = 1; i <= size.z; i++)
    for (size_t j = 1; j <= size.x; j++) {
      Point<size_t> pos(j, 0, i);
      currentArea->set(calculatePhiOnBorder(pos.add(rank * size.x, 0, 0)), pos);
      currentArea->set(
          calculatePhiOnBorder(pos.add(rank * size.x, size.y + 1, 0)),
          pos.add(0, size.y + 1, 0));
    }

  // Противоположные плоскости y->x
  for (size_t i = 1; i <= size.y; i++)
    for (size_t j = 1; j <= size.x; j++) {
      Point<size_t> pos(j, i, 0);
      currentArea->set(calculatePhiOnBorder(pos.add(rank * size.x, 0, 0)), pos);
      currentArea->set(
          calculatePhiOnBorder(pos.add(rank * size.x, 0, size.z + 1)),
          pos.add(0, 0, size.z + 1));
    }

  if (!borderUpper && !borderLower)
    return;

  // Если подобласть процесса на границе, заполняем верхние и нижние плоскости
  for (size_t i = 1; i <= size.y; i++)
    for (size_t j = 1; j <= size.z; j++) {
      Point<size_t> pos(0, i, j);
      if (borderLower)
        currentArea->set(calculatePhiOnBorder(pos.add(rank * size.x, 0, 0)),
                         pos);
      if (borderUpper)
        currentArea->set(
            calculatePhiOnBorder(pos.add((rank + 1) * size.x + 1, 0, 0)),
            pos.add(size.x + 1, 0, 0));
    }
}

double SolveData::calculatePhiOnBorder(const Point<int> pos) {
  double x = center.x + pos.x * height.x;
  double y = center.y + pos.y * height.y;
  double z = center.z + pos.z * height.z;
  return x * x + y * y + z * z;
}

double SolveData::calculateRo(const Point<int> pos) {
  return 6 - paramA * calculatePhiOnBorder(pos);
}

double SolveData::calculateNextPhiAt(const Point<int> pos) {
  double powHx = height.x * height.x;
  double powHy = height.y * height.y;
  double powHz = height.z * height.z;

  double first = currentArea->get(pos.add(1, 0, 0)) +
                 currentArea->get(pos.add(-1, 0, 0));

  double second = currentArea->get(pos.add(0, 1, 0)) +
                  currentArea->get(pos.add(0, -1, 0));

  double third = currentArea->get(pos.add(0, 0, 1)) +
                 currentArea->get(pos.add(0, 0, -1));

  double divider = 2 / powHx + 2 / powHy + 2 / powHz + paramA;
  double ro = calculateRo(pos);

  first /= powHx;
  second /= powHy;
  third /= powHz;

  return (first + second + third - ro) / divider;
}

void SolveData::calculateConcurrentBorders() {
  const Point<size_t>& size = currentArea->size;

  if (borderLower && borderUpper)
    return;

  for (int i = 1; i <= static_cast<int>(size.y); i++)
    for (int j = 1; j <= static_cast<int>(size.z); j++) {
      Point<int> pos(1, i, j);
      if (!borderLower)
        nextArea->set(calculateNextPhiAt(pos.add(rank * size.x, 0, 0)), pos);
      if (!borderUpper)
        nextArea->set(calculateNextPhiAt(pos.add((rank + 1) * size.x, 0, 0)),
                      pos.add(size.x - 1, 0, 0));
    }
}

void SolveData::sendBorders() {
  const Point<size_t>& size = currentArea->size;

  if (!borderLower) {
    MPI_Isend(currentArea->getFlatSlice(1), (size.y + 2) * (size.z + 2),
              MPI_DOUBLE, rank - 1, 123, MPI_COMM_WORLD, &sendRequests[0]);
    MPI_Irecv(nextArea->getFlatSlice(0), (size.y + 2) * (size.z + 2),
              MPI_DOUBLE, rank - 1, 123, MPI_COMM_WORLD, &recvRequests[0]);
  }
  if (!borderUpper) {
    MPI_Isend(currentArea->getFlatSlice(size.x), (size.y + 2) * (size.z + 2),
              MPI_DOUBLE, rank + 1, 123, MPI_COMM_WORLD, &sendRequests[1]);
    MPI_Irecv(nextArea->getFlatSlice(size.x + 1), (size.y + 2) * (size.z + 2),
              MPI_DOUBLE, rank + 1, 123, MPI_COMM_WORLD, &recvRequests[1]);
  }
}

void SolveData::calculateCenter() {
  const Point<size_t>& size = currentArea->size;
  for (int i = 2; i < static_cast<int>(size.x) - 2; i++)
    for (int j = 1; j < static_cast<int>(size.y); j++)
      for (int k = 1; k < static_cast<int>(size.z); k++) {
        Point<int> pos(i, j, k);
        nextArea->set(calculateNextPhiAt(pos.add(rank * size.x, 0, 0)), pos);
      }
}

bool SolveData::needNext() {
  double max = 0;
  double value;
  double allMax[proc_count];
  const Point<size_t>& size = currentArea->size;

  for (size_t i = 1; i < size.x; i++)
    for (size_t j = 1; j < size.y; j++)
      for (size_t k = 1; k < size.z; k++) {
        Point<size_t> pos(i, j, k);
        value = abs(currentArea->get(pos) - nextArea->get(pos));
        if (max < value)
          max = value;
      }

  MPI_Allgather(&max, 1, MPI_DOUBLE, allMax, 1, MPI_DOUBLE, MPI_COMM_WORLD);
  for (size_t i = 0; i < proc_count; i++)
    if (max < allMax[i])
      max = allMax[i];

  return max < epsilon;
}

void SolveData::waitCommunication() {
  if (!borderUpper) {
    MPI_Wait(&recvRequests[1], MPI_STATUS_IGNORE);
    MPI_Wait(&sendRequests[1], MPI_STATUS_IGNORE);
  }
  if (!borderLower) {
    MPI_Wait(&recvRequests[0], MPI_STATUS_IGNORE);
    MPI_Wait(&sendRequests[0], MPI_STATUS_IGNORE);
  }
}

void SolveData::prepareNext() {
  currentArea->swapAreas(*nextArea);
}

void SolveData::dumpIteration() {
  const Point<size_t>& size = currentArea->size;
  std::cout << std::fixed;
  std::cout.precision(1);
  for (size_t x = 0; x < size.x + 2; x++) {
    for (size_t y = 0; y < size.y + 2; y++) {
      for (size_t z = 0; z < size.z + 2; z++) {
        double value = currentArea->get(Point<size_t>(x, y, z));
        if (std::isnan(value)) {
          std::cout << " \e[0;31m" << value << "\e[0m " << std::flush;
          continue;
        }
        if (value >= 0)
          std::cout << ' ' << std::flush;
        if (abs(value) >= epsilon) {
          if (value > 0)
            std::cout << "\e[0;32m" << std::flush;
          else
            std::cout << "\e[0;36m" << std::flush;
          std::cout << value << "\e[0m" << std::flush;
        } else
          std::cout << value << std::flush;
      }
      std::cout << "\n" << std::endl;
    }
    std::cout << "=================================== x:" << x << std::endl;
  }
}
