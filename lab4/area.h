
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
  Area(const Point<size_t> size, const  double initial_approx);

  /**
   * Получает значение из области.
   */
  double get(const Point<int> pos)  const noexcept;

  /**
   * Записывает значение в область.
   * Границы включаются.
   */
  void set(const double value, const Point<size_t> pos) noexcept;

  /**
   * Обменивается значениями с другой областью.
   * Используется для перехода на новый шаг.
   *
   * @param area Обмениваемая область.
   */
  void swapAreas(Area& area) noexcept;

  /**
   * Копирует значения из другой области.
   *
   * @param area Область копии.
   */
  void copyData(const Area& area);

  /**
   * Возвращает указатель на плоскость, разрезанной по X подобласти.
   *
   * @param num Номер плоскости.
   */
  double* getFlatSlice(const size_t num) const;

  ~Area();

 private:
  double* area;
};

#endif

