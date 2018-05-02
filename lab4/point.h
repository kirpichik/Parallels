
#ifndef _POINT_H
#define _POINT_H

#include <cstddef>
#include <iostream>

template <typename T>
struct Point {
  T x;
  T y;
  T z;

  Point() {}
  Point(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}

  template <typename C>
  operator Point<C>() const {
    return Point<C>(static_cast<C>(x), static_cast<C>(y), static_cast<C>(z));
  }

  Point<T> add(const T& x, const T& y, const T& z) const {
    return Point(this->x + x, this->y + y, this->z + z);
  }
};

#endif

