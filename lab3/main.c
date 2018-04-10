#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <sys/time.h>

// Размеры решетки
#define P0 2
#define P1 2

// Размеры матриц: A = MxN, B = NxK, C = MxK
#define M 8
#define N 8
#define K 8

void mult_matrix_parts(double*, double*, double*, size_t, size_t, size_t);
void calc_matrix_sub_data(int**, int**, int**, int**, int[2], size_t);
void build_mpi_types(size_t, size_t, size_t, size_t, MPI_Datatype*, MPI_Datatype*);
void matrix_mult_on_grid(int[3], int[2], double*, double*, double*, MPI_Comm);
void create_matrixes(double**, double**, double**);
void print_matrix(double*, size_t, size_t);

int main(int argc, char** argv) {
  int size, rank;
  int matrixes_sizes[3] = { M, N, K };
  int grid_size[2] = { P0, P1 };
  // Размеры решетки
  int dims[2] = { 0, 0 };
  // Периодичность решетки
  int periods[2] = { 0, 0 };

  struct timeval tv1, tv2;
  MPI_Comm comm;

  double* matrix_A = NULL;
  double* matrix_B = NULL;
  double* matrix_C = NULL;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (P0 * P1 != size) {
    printf("Required processes count: %d", P0 * P1);
    MPI_Finalize();
    return 0;
  }

  // Создание размеров решетки и самой топологии решетки
  MPI_Dims_create(size, 2, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // На начальном элементе генерируем матрицы
  if (!rank)
    create_matrixes(&matrix_A, &matrix_B, &matrix_C);

  gettimeofday(&tv1, NULL);

  // Выполняем работу
  matrix_mult_on_grid(matrixes_sizes, grid_size, matrix_A, matrix_B, matrix_C, comm);

  gettimeofday(&tv2, NULL);
  int exec_time = (tv2.tv_sec - tv1.tv_sec) * 1000000 + tv2.tv_usec - tv1.tv_usec;
  printf("rank = %d, time = %d\n", rank, exec_time);

  // На начальном элементе выводим получившуюся матрицу и освобождаем память
  if (!rank) {
    print_matrix(matrix_C, M, K);

    free(matrix_A);
    free(matrix_B);
    free(matrix_C);
  }

  MPI_Comm_free(&comm);
  MPI_Finalize();
  return 0;
}

/**
 * Печатает матрицу.
 *
 * @param matrix Матрица.
 * @param width Ширина.
 * @param height Высота.
 */
void print_matrix(double* matrix, size_t width, size_t height) {
  for (size_t i = 0; i < width; i++) {
      for (size_t j = 0; j < height; j++)
        printf(" %3.1f", matrix[K * i + j]);
      printf("\n");
    }
}

/**
 * Выделяет память и генерирует содержимое матриц для умножения.
 *
 * @param matrix_A Матрица A для заполнения.
 * @param matrix_B Матрица B для заполнения.
 * @param matrix_C Матрица C для заполнения.
 */
void create_matrixes(double** matrix_A, double** matrix_B, double** matrix_C) {
  (*matrix_A) = (double*) malloc(M * N * sizeof(double));
  (*matrix_B) = (double*) malloc(N * K * sizeof(double));
  (*matrix_C) = (double*) malloc(M * K * sizeof(double));

  for (size_t i = 0; i < M; i++)
    for (size_t j = 0; j < N; j++)
      (*matrix_A)[N * i + j] = 1;

  for (size_t i = 0; i < N; i++)
    for (size_t j = 0; j < K; j++)
      (*matrix_B)[K * i + j] = 1;
}

/**
 * Создает дополнительные типы матриц для удобной передачи между процессами.
 *
 * @param size_N Размеры матриц.
 * @param size_K Размер матриц.
 * @param strip_A Размер куска A.
 * @param strip_B Размер куска B.
 * @param typeb Результат создания типа для кусков B.
 * @param typec Результат создания типа для кусков C.
 */
void build_mpi_types(size_t size_N, size_t size_K, size_t strip_A, size_t strip_B, MPI_Datatype* typeb, MPI_Datatype* typec) {
  MPI_Datatype types[2];
  int blen[2] = { 1, 1 };
  long size, disp[2];

  // Создаем тип для передачи кусков B
  MPI_Type_vector(size_N, strip_B, size_K, MPI_DOUBLE, &types[0]);
  MPI_Type_extent(MPI_DOUBLE, &size);
  disp[0] = 0;
  disp[1] = size * strip_B;
  types[1] = MPI_UB;
  MPI_Type_struct(2, blen, disp, types, typeb);
  MPI_Type_commit(typeb);

  // Создаем тип для передачи ячеек C
  MPI_Type_vector(strip_A, strip_B, size_K, MPI_DOUBLE, &types[0]);
  MPI_Type_struct(2, blen, disp, types, typec);
  MPI_Type_commit(typec);

  MPI_Type_free(&types[0]);
}

/**
 * Подсчет данных частей матриц для передачи между процессами.
 *
 * @param b_sizes Размеры кусков B.
 * @param b_displs Смещения кусков B.
 * @param c_sizes Размеры кусков C.
 * @param c_displs Смещения кусков C.
 * @param grid Параметры решетки.
 * @param strip_A Размер куска A для подсчета кусков C.
 */
void calc_matrix_sub_data(int** b_sizes, int** b_displs, int** c_sizes, int** c_displs, int grid[2], size_t strip_A) {
  size_t grid_width = grid[0];
  size_t grid_height = grid[1];

  // Вычисляем размеры всех подматриц для B
  (*b_displs) = (int*) malloc(grid_height * sizeof(int));
  (*b_sizes) = (int*) malloc(grid_height * sizeof(int));
  for (size_t j = 0; j < grid_height; j++) {
    (*b_displs)[j] = j;
    (*b_sizes)[j] = 1;
  }

  // Размеры всех подматриц для C
  (*c_displs) = (int*) malloc(grid_width * grid_height * sizeof(int));
  (*c_sizes) = (int*) malloc(grid_width * grid_height * sizeof(int));
  for (size_t i = 0; i < grid_width; i++)
    for (size_t j = 0; j < grid_height; j++) {
      (*c_displs)[i * grid_height + j] = i * grid_height * strip_A + j;
      (*c_sizes)[i * grid_height + j] = 1;
    }
}

/**
 * Умножение частей матриц.
 *
 * @param part_A Часть матрицы A.
 * @param part_B Часть матрицы B.
 * @param part_C Хранилище результатов.
 * @param strip_A Размер части матрицы A.
 * @param strip_B Размер части матрицы B.
 * @param size_N Параметр матрицы A.
 */
void mult_matrix_parts(double* part_A, double* part_B, double* part_C, size_t strip_A, size_t strip_B, size_t size_N) {
  for (size_t i = 0; i < strip_A; i++)
    for (size_t j = 0; j < strip_B; j++) {
      part_C[strip_B * i + j] = 0;
      for (size_t k = 0; k < size_N; k++)
        part_C[strip_B * i + j] += + part_A[size_N * i + j] * part_B[strip_B * k + j];
    }
}

/**
 * Умножение матриц на решетке.
 *
 * @param sizes Размеры матриц.
 * @param grid_sizes Размеры решетки.
 * @param matrix_A Матрица A.
 * @param matrix_B Матрица B.
 * @param matrix_C Матрица C.
 * @param comm Коммуникатор.
 */
void matrix_mult_on_grid(int sizes[3], int grid_sizes[2], double* matrix_A, double* matrix_B, double* matrix_C, MPI_Comm comm) {
  // Размеры топологии
  size_t grid_width = grid_sizes[0];
  size_t grid_height = grid_sizes[1];
  // Размеры матриц
  size_t size_M = sizes[0];
  size_t size_N = sizes[1];
  size_t size_K = sizes[2];
  // Размеры полосок матриц
  size_t strip_A, strip_B;
  // Куски матриц
  double* part_A;
  double* part_B;
  double* part_C;
  // Координаты и ранг в решетке
  int coords[2];
  int rank;
  // Смещения и размеры подматриц B и C для передачи
  int* sub_C_sizes;
  int* sub_C_displs;
  int* sub_B_sizes;
  int* sub_B_displs;
  // Расширение типов частей для пересылки данных
  MPI_Datatype typeb = 0;
  MPI_Datatype typec = 0;
  // Коммуникаторы для полной решетки, подрешетки и копия исходного
  MPI_Comm comm_2D;
  MPI_Comm comm_1D[2];
  MPI_Comm comm_copy;
  // Дополнительные переменные для создания карты
  int periods[2] = { 0, 0 };
  int remains[2];

  // Копируем старый коммуникатор
  MPI_Comm_dup(comm, &comm_copy);

  // Раздаем всем параметры матриц и размеры сети (от имени начального)
  MPI_Bcast(sizes, 3, MPI_INT, 0, comm_copy);
  MPI_Bcast(grid_sizes, 2, MPI_INT, 0, comm_copy);

  // Создаем карту сети
  MPI_Cart_create(comm_copy, 2, grid_sizes, periods, 0, &comm_2D);

  // Получаем наш ранг и координаты в решетке
  MPI_Comm_rank(comm_2D, &rank);
  MPI_Cart_coords(comm_2D, rank, 2, coords);

  // Разбиваем коммуникаторы на одномерные подрешетки
  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < 2; j++)
      remains[j] = (i == j);
    MPI_Cart_sub(comm_2D, remains, &comm_1D[i]);
  }

  // Вычисляем размеры кусков-полосок
  strip_A = size_M / grid_width;
  strip_B = size_K / grid_height;

  part_A = (double*) malloc(strip_A * size_N * sizeof(double));
  part_B = (double*) malloc(strip_B * size_N * sizeof(double));
  part_C = (double*) malloc(strip_A * strip_B * sizeof(double));

  if (!rank) {
    // Создаем типы для удобной передачи кусков матриц
    build_mpi_types(size_N, size_K, strip_A, strip_B, &typeb, &typec);

    // Вычисляем размеры всех подматриц
    calc_matrix_sub_data(&sub_B_sizes, &sub_B_displs, &sub_C_sizes, &sub_C_displs, grid_sizes, strip_A);
  }

  // Нулевая ветвь передает горизонтальные полосы матрицы A по x координате
  if (coords[1] == 0)
    MPI_Scatter(matrix_A, strip_A * size_N, MPI_DOUBLE, part_A, strip_A * size_N, MPI_DOUBLE, 0, comm_1D[0]);

  // Нулевая ветвь передает горизонтальные полосы матрицы B по y координате
  if (coords[0] == 0)
    MPI_Scatterv(matrix_B, sub_B_sizes, sub_B_displs, typeb, part_B, size_N * strip_B, MPI_DOUBLE, 0, comm_1D[1]);

  // Передаем подматрицы матрицы A по y плоскости
  MPI_Bcast(part_A, strip_A * size_N, MPI_DOUBLE, 0, comm_1D[1]);
  // Передаем подматрицы матрицы B по x плоскости
  MPI_Bcast(part_B, strip_B * size_N, MPI_DOUBLE, 0, comm_1D[0]);

  // Выполняем умножение нашего куска подматриц
  mult_matrix_parts(part_A, part_B, part_C, strip_A, strip_B, size_N);

  // Собираем все куски в начале
  MPI_Gatherv(part_C, strip_A * strip_B, MPI_DOUBLE, matrix_C, sub_C_sizes, sub_C_displs, typec, 0, comm_2D);

  free(part_A);
  free(part_B);
  free(part_C);
  MPI_Comm_free(&comm_copy);
  MPI_Comm_free(&comm_2D);

  for (size_t i = 0; i < 2; i++)
    MPI_Comm_free(&comm_1D[i]);

  if (!rank) {
    free(sub_C_sizes);
    free(sub_C_displs);
    MPI_Type_free(&typeb);
    MPI_Type_free(&typec);
  }
}

