#ifndef _SOLVE_DATA
#define _SOLVE_DATA

#include "area.h"
#include "point.h"

class SolveData {
 public:
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

