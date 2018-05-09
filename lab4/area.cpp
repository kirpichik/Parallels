
#include <algorithm>

#include "area.h"

Area::Area(const Point<size_t> size, double initial_approx) : size(size) {
  size_t mem = (size.x + 2) * (size.y + 2) * (size.z + 2);
  area = new double[mem];
  std::fill_n(area, mem, initial_approx);
}

Area::~Area() {
  delete[] area;
}

double Area::get(Point<int> pos) const {
  return area[pos.x * (size.y + 2) * (size.z + 2) + pos.y * (size.z + 2) + pos.z];
}

void Area::set(double value, Point<size_t> pos) {
  pos.x++;
  pos.y++;
  pos.z++;
  justSet(value, pos);
}

void Area::justSet(double value, Point<size_t> pos) {
  area[pos.x * (size.y + 2) * (size.z + 2) + pos.y * (size.z + 2) + pos.z] = value;
}

void Area::swapAreas(Area& area) {
  double* arr = this->area;
  this->area = area.area;
  area.area = arr;
}

double* Area::getFlatSlice(size_t num) const {
  return area + (num * (size.y + 2) * (size.z + 2));
}

