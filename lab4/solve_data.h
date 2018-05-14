#ifndef _SOLVE_DATA
#define _SOLVE_DATA

#include "area.h"
#include "point.h"

class SolveData {
 public:
  SolveData(const size_t proc_count,
            const size_t rank,
            const long double epsilon,
            const long double paramA);

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
   * Дожидается приема границ от соседей и отправки границ соседям.
   */
  void waitCommunication();

  /**
   * Подготавливает следующий шаг.
   */
  void prepareNext();

  /**
   * Распечатывает доступную область данного ранка.
   */
  void dumpIteration();

  ~SolveData();

 private:
  Point<long double> center;
  Point<long double> distance;
  Point<long double> height;
  Point<size_t> grid;
  size_t rank;
  size_t proc_count;
  long double paramA;
  long double epsilon;
  long double initial_approx;
  Area* currentArea;
  Area* nextArea;
  bool borderUpper;
  bool borderLower;
  MPI_Request sendRequests[2];
  MPI_Request recvRequests[2];

  /**
   * Инициализирует границы области начальными значениями.
   */
  void initBorders();

  /**
   * Вычисляет значение функции фи на границе области.
   */
  long double calculatePhiOnBorder(const Point<int> pos) const noexcept;

  /**
   * Вычисляет значение функции фи методом приближения внутри области.
   */
  long double calculateNextPhiAt(const Point<int> pos) const noexcept;

  /**
   * Вычисляет значение функции ро.
   */
  long double calculateRo(const Point<int> pos) const noexcept;
};

#endif
