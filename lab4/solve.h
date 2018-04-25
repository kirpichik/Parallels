
#ifndef _SOLVE_H
#define _SOLVE_H

#include <vector>

template <typename T>
struct Point {
  T x;
  T y;
  T z;

  Point() {}
  Point(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}

  template <typename C>
  operator Point<C> () const {
    return Point<C>((C) x, (C) y, (C) z);
  }

  Point<T> add(const T& x, const T& y, const T& z) {
    return Point(this->x + x, this->y + y, this->z + z);
  }
};

class Area;

struct SolveData {
  Point<double> center;
  Point<double> distance;
  Point<double> height;
  Point<size_t> grid;
  double paramA;
  double epsilon;
  double initial_approx;
  Area* currentArea;
  Area* nextArea;
  bool borderUpper;
  bool borderLower;
  size_t rank;

  SolveData(size_t proc_count, size_t rank);

  void calculateConcurrentBorders();

  void sendBorders();

  void calculateCenter();

  bool needNext();

  void prepareNextStep();

  ~SolveData();

 private:
  void initBorders();

  double calculatePhiOnBorder(Point<int> pos);

  double calculateNextPhiAt(Point<int> pos);

  double calculateRo(Point<int> pos);
};

/**
 * Подобласть
 */
class Area {
 public:
  const Point<size_t> size;

  /**
   * Выделяет память под x * y * z * 2^3 элементов (включая границы)
   *
   * @param initial_approx Начальное приближение,
   * которым нужно проинициализировать область.
   */
  Area(const Point<size_t> size, double initial_approx);

  /**
   * Получает значение из области.
   * Значения берутся исключая границы.
   * Значения могут быть от -1 до (_размер_ + 1).
   * В крайних случаях берутся обсчитанные границы.
   */
  double get(Point<int> pos) const;

  /**
   * Записывает значение в область.
   * Значения берутся исключая границы.
   * Данным методом нельзя записать граничные значения.
   */
  void set(double value, Point<size_t> pos);

  /**
   * Записывает значение в область.
   * Границы включаются.
   */
  void justSet(double value, Point<size_t> pos);

  /**
   * Обменивается значениями с другой областью.
   * Используется для перехода на новый шаг.
   *
   * @param area Обмениваемая область.
   */
  void swapAreas(Area& area);

  ~Area();

 private:
  double* area;
};

#endif

