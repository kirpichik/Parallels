
#ifndef _AREA_H
#define _AREA_H

#include "point.h"

/**
 * Подобласть
 */
struct Area {

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

  /**
   * Возвращает указатель на плоскость, разрезанной по X подобласти.
   *
   * @param num Номер плоскости.
   */
  double* getFlatSlice(size_t num) const;

  ~Area();

 private:
  double* area;
};

#endif

