
#include <cstring>
#include <cmath>

#include "solve.h"

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

  if (grid.z % proc_count != 0)
    throw "Invalid processes count.";

  Point<size_t> size(grid.x, grid.y, grid.z / proc_count);
  currentArea = new Area(size, initial_approx);
  nextArea = new Area(size, initial_approx);
  borderUpper = !rank;
  borderLower = rank + 1 == proc_count;

  initBorders();
}

SolveData::~SolveData() {
  delete currentArea;
  delete nextArea;
}

Area::Area(const Point<size_t> size, double initial_approx) : size(size) {
  size_t mem = (size.x + 2) * (size.y + 2) * (size.z + 2);
  area = new double[mem];
  // TODO - Инициализация в случае иного начального приближения
  memset(area, 0, sizeof(double) * mem);
}

Area::~Area() {
  delete[] area;
}

double Area::get(Point<int> pos) const {
  pos.x++;
  pos.y++;
  pos.z++;
  return area[pos.x * size.y * size.z + pos.y * size.z + pos.z];
}

void Area::set(double value, Point<size_t> pos) {
  pos.x++;
  pos.y++;
  pos.z++;
  justSet(value, pos);
}

void Area::justSet(double value, Point<size_t> pos) {
  area[pos.x * size.y * size.z + pos.y * size.z + pos.z] = value;
}

void Area::swapAreas(Area& area) {
  double* arr = this->area;
  this->area = area.area;
  area.area = arr;
}

void SolveData::initBorders() {
  const Point<size_t>& size = currentArea->size;

  // Противоположные плоскости x->y
  for (size_t i = 0; i < size.x + 2; i++)
    for (size_t j = rank * size.z; j < (rank + 1) * size.z; j++) {
      Point<size_t> pos(i, 0, j);
      currentArea->justSet(calculatePhiOnBorder(pos), pos);

      currentArea->justSet(calculatePhiOnBorder(pos.add(0, size.y, 0)), pos);
    }

  // Противоположные плоскости z->y
  for (size_t i = 0; i < size.y + 2; i++)
    for (size_t j = rank * size.z; j < (rank + 1) * size.z; j++) {
      Point<size_t> pos(0, i, j);
      currentArea->justSet(calculatePhiOnBorder(pos), pos);

      currentArea->justSet(calculatePhiOnBorder(pos.add(size.x, 0, 0)), pos);
    }

  if (!borderUpper && !borderLower)
    return;

  // Если подобласть процесса на границе, заполняем верхние и нижние плоскости
  for (size_t i = 0; i < size.x + 2; i++)
    for (size_t j = 0; j < size.y + 2; j++) {
      Point<size_t> pos(i, j, 0);
      if (borderUpper)
        currentArea->justSet(calculatePhiOnBorder(pos), pos);
      if (borderLower)
        currentArea->justSet(calculatePhiOnBorder(pos.add(0, 0, distance.z)), pos);
    }
}

double SolveData::calculatePhiOnBorder(Point<int> pos) {
  double x = center.x + pos.x * height.x;
  double y = center.y + pos.y * height.y;
  double z = center.z + pos.z * height.z;
  return x * x + y * y + z * z;
}

double SolveData::calculateRo(Point<int> pos) {
  return 6 - paramA * currentArea->get(pos);
}

double SolveData::calculateNextPhiAt(Point<int> pos) {
  double powHx = pow(height.x, 2);
  double powHy = pow(height.y, 2);
  double powHz = pow(height.z, 2);

  double first  = currentArea->get(pos.add(1, 0, 0)) -
                  currentArea->get(pos) * 2 +
                  currentArea->get(pos.add(-1, 0, 0));

  double second = currentArea->get(pos.add(0, 1, 0)) -
                  currentArea->get(pos) * 2 +
                  currentArea->get(pos.add(0, -1, 0));

  double third  = currentArea->get(pos.add(0, 0, 1)) -
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
  for (int i = 0; i < size.x; i++)
    for (int j = 0; j < size.y; j++) {
      Point<int> pos(i, j, rank * size.z);
      nextArea->set(calculateNextPhiAt(pos), pos);
      nextArea->set(calculateNextPhiAt(pos), pos.add(0, 0, size.z));
    }
}

void SolveData::sendBorders() {

}

void SolveData::calculateCenter() {
  const Point<size_t>& size = currentArea->size;
  for (int i = 0; i < size.x; i++)
    for (int j = 0; j < size.y; j++)
      for (int k = rank * size.z; k < (rank + 1) * size.z; k++) {
        Point<int> pos(i, j, k);
        nextArea->set(calculateNextPhiAt(pos), pos);
      }
}

bool SolveData::needNext() {
  return false;
}

void SolveData::prepareNextStep() {

}

