
#ifndef _SOLVE_H
#define _SOLVE_H

#include <mpi.h>

template <typename T>
struct Point {
  T x;
  T y;
  T z;

  Point() {}
  Point(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}

  template <typename C>
  operator Point<C>() const {
    return Point<C>((C)x, (C)y, (C)z);
  }

  Point<T> add(const T& x, const T& y, const T& z) const {
    return Point(this->x + x, this->y + y, this->z + z);
  }
};

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

  ~Area();

 private:
  double* area;
};

struct SolveData {

  SolveData(size_t proc_count, size_t rank);

  /**
   * Обсчитывает следующий шаг для граничащих подобластей,
   * чтобы после передать их соседним процессам.
   */
  void calculateConcurrentBorders();

  /**
   * Отправляет граничащие области соседям.
   */
  void sendBorders();

  /**
   * Обсчитывает центральную область.
   */
  void calculateCenter();

  /**
   * Проверяет, нужно ли продолжать вычисления.
   */
  bool needNext();

  /**
   * Дожидается приема границ от соседей и подготавливает области
   * для обсчета следующего шага.
   */
  void prepareNextStep();

  ~SolveData();

 private:
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
  size_t proc_count;
  MPI_Request sendRequests[2];
  MPI_Request recvRequests[2];

  /**
   * Инициализирует границы области начальными значениями.
   */
  void initBorders();

  /**
   * Вычисляет значение функции фи на границе области.
   */
  double calculatePhiOnBorder(const Point<int> pos);

  /**
   * Вычисляет значение функции фи методом приближения внутри области.
   */
  double calculateNextPhiAt(const Point<int> pos);

  /**
   * Вычисляет значение функции ро.
   */
  double calculateRo(const Point<int> pos);
};

#endif

