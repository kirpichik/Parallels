
#include "area.h"

Area::Area(const Point<size_t> size, double initial_approx) : size(size) {
  size_t mem = (size.x + 2) * (size.y + 2) * (size.z + 2);
  area = new double[mem]{initial_approx};
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

double* Area::getFlatSlice(size_t num) const {
  return area + (num * size.y * size.z);
}

