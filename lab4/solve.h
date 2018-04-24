
#ifndef _SOLVE_H
#define _SOLVE_H

#include <vector>

template <typename T>
struct Point {
  T x;
  T y;
  T z;
};

struct SolveData {
  SolveData(size_t proc_count, size_t rank);

  Point<double> center;
  Point<double> distance;
  Point<double> height;
  Point<size_t> grid;
  double paramA;
  double epsilon;
  Area* currentArea;
  Area* nextArea;
  bool borderUpper;
  bool borderLower;
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
   * @param fill_zero Проинициализировать ли массив нулями.
   */
  Area(size_t x, size_t y, size_t z, bool fill_zero);

  /**
   * Получает значение из области.
   * Значения берутся исключая границы.
   * Значения могут быть от -1 до (_размер_ + 1).
   * В крайних случаях берутся обсчитанные границы.
   */
  double get(int x, int y, int z) const;

  /**
   * Записывает значение в область.
   * Значения берутся исключая границы.
   * Данным методом нельзя записать граничные значения.
   */
  void set(double value, size_t x, size_t y, size_t z);

  /**
   * Записывает значение в область.
   * Границы включаются.
   */
  void just_set(double value, size_t x, size_t y, size_t z);

  /**
   * Обменивается значениями с другой областью.
   * Используется для перехода на новый шаг.
   *
   * @param area Обмениваемая область.
   */
  void swap(const Area& area);

 private:
  double* area;
};

#endif

