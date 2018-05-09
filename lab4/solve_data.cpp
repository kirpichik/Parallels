
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
  grid = Point<size_t>(16, 16, 16);
  // h_x, h_y, h_z
  height.x = distance.x / (grid.x - 1);
  height.y = distance.y / (grid.y - 1);
  height.z = distance.z / (grid.z - 1);

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
  borderUpper = !rank;
  borderLower = (rank + 1) == proc_count;

  initBorders();
}

SolveData::~SolveData() {
  delete currentArea;
  delete nextArea;
}

void SolveData::initBorders() {
  const Point<size_t>& size = currentArea->size;

  // Противоположные плоскости z->x
  for (size_t i = 1; i < size.z; i++)
    for (size_t j = 1; j < size.x; j++) {
      Point<size_t> pos(j, 0, i);
      currentArea->justSet(calculatePhiOnBorder(pos.add(rank * size.x, 0, 0)), pos);
      currentArea->justSet(calculatePhiOnBorder(pos.add(rank * size.x, size.y, 0)), pos.add(0, size.y, 0));
    }

  // Противоположные плоскости y->x
  for (size_t i = 1; i < size.y; i++)
    for (size_t j = 1; j < size.x; j++) {
      Point<size_t> pos(j, i, 0);
      currentArea->justSet(calculatePhiOnBorder(pos.add(rank * size.x, 0, 0)), pos);
      currentArea->justSet(calculatePhiOnBorder(pos.add(rank * size.x, 0, size.z)), pos.add(0, 0, size.z));
    }

  if (!borderUpper && !borderLower)
    return;

  // Если подобласть процесса на границе, заполняем верхние и нижние плоскости
  for (size_t i = 1; i < size.y; i++)
    for (size_t j = 1; j < size.z; j++) {
      Point<size_t> pos(0, i, j);
      if (borderUpper)
        currentArea->justSet(calculatePhiOnBorder(pos.add(rank * size.x, 0, 0)), pos);
      if (borderLower)
        currentArea->justSet(calculatePhiOnBorder(pos.add((rank + 1) * size.x, 0, 0)), pos.add(size.x, 0, 0));
    }
}

double SolveData::calculatePhiOnBorder(const Point<int> pos) {
  double x = center.x + pos.x * height.x;
  double y = center.y + pos.y * height.y;
  double z = center.z + pos.z * height.z;
  return x * x + y * y + z * z;
}

double SolveData::calculateRo(const Point<int> pos) {
  return 6 - paramA * currentArea->get(pos);
}

double SolveData::calculateNextPhiAt(const Point<int> pos) {
  double powHx = pow(height.x, 2);
  double powHy = pow(height.y, 2);
  double powHz = pow(height.z, 2);

  double first = currentArea->get(pos.add(1, 0, 0)) -
                 currentArea->get(pos) * 2 +
                 currentArea->get(pos.add(-1, 0, 0));

  double second = currentArea->get(pos.add(0, 1, 0)) -
                  currentArea->get(pos) * 2 +
                  currentArea->get(pos.add(0, -1, 0));

  double third = currentArea->get(pos.add(0, 0, 1)) -
                 currentArea->get(pos) * 2 +
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

  for (int i = 1; i < static_cast<int>(size.y); i++)
    for (int j = 1; j < static_cast<int>(size.z); j++) {
      Point<int> pos(1, i, j);
      nextArea->justSet(calculateNextPhiAt(pos.add(rank * size.x, 0, 0)), pos);
      nextArea->justSet(calculateNextPhiAt(pos.add((rank + 1) * size.x, 0, 0)), pos.add(size.x - 1, 0, 0));
    }
}

void SolveData::sendBorders() {
  const Point<size_t>& size = currentArea->size;

  if (!borderLower) {
    MPI_Isend(currentArea->getFlatSlice(1), size.y * size.z, MPI_DOUBLE,
              rank + 1, 123, MPI_COMM_WORLD, &sendRequests[0]);
    MPI_Irecv(nextArea->getFlatSlice(0), size.y * size.z, MPI_DOUBLE, rank + 1,
              123, MPI_COMM_WORLD, &recvRequests[0]);
  }
  if (!borderUpper) {
    MPI_Isend(currentArea->getFlatSlice(size.x), size.y * size.z,
              MPI_DOUBLE, rank - 1, 123, MPI_COMM_WORLD, &sendRequests[1]);
    MPI_Irecv(nextArea->getFlatSlice(size.x + 1), size.y * size.z, MPI_DOUBLE,
              rank - 1, 123, MPI_COMM_WORLD, &recvRequests[1]);
  }
}

void SolveData::calculateCenter() {
  const Point<size_t>& size = currentArea->size;
  for (int i = 0; i < static_cast<int>(size.x); i++)
    for (int j = 0; j < static_cast<int>(size.y); j++)
      for (int k = 0; k < static_cast<int>(size.z); k++) {
        Point<int> pos(i, j, k);
        nextArea->set(calculateNextPhiAt(pos), pos.add(rank * size.x, 0, 0));
      }
}

bool SolveData::needNext() {
  double max = 0;
  double value;
  double allMax[proc_count];
  const Point<size_t>& size = currentArea->size;

  for (size_t i = 0; i < size.x; i++)
    for (size_t j = 0; j < size.y; j++)
      for (size_t k = 0; k < size.z; k++) {
        Point<size_t> pos(i, j, k);
        value = abs(currentArea->get(pos) - nextArea->get(pos));
        if (max > value)
          max = value;
      }

  // TODO - malloc: *** incorrect checksum for freed object - object was probably modified after being freed.
  //MPI_Allgather(&max, 1, MPI_DOUBLE, allMax, 1, MPI_DOUBLE, MPI_COMM_WORLD);
  for (size_t i = 0; i < proc_count; i++)
    if (max > allMax[i])
      max = allMax[i];

  return max < epsilon;
}

void SolveData::prepareNextStep() {
  if (!borderLower) {
    MPI_Wait(&recvRequests[0], MPI_STATUS_IGNORE);
    //MPI_Wait(&sendRequests[0], MPI_STATUS_IGNORE);
  }
  if (!borderUpper) {
    MPI_Wait(&recvRequests[1], MPI_STATUS_IGNORE);
    //MPI_Wait(&sendRequests[1], MPI_STATUS_IGNORE);
  }
  currentArea->swapAreas(*nextArea);
}

