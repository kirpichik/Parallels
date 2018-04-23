
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct point {
  double x;
  double y;
  double z;
} point_t;

typedef struct calc_data {
  double* current_area;
  double* next_area;
  point_t area_size;
  point_t center;
  point_t distance;
  point_t disc_grid;
  point_t height;
  double paramA;
  double epsilon;
  double initial_approx;
} calc_data_t;

/**
 * Заполняет начальные данные каждого процесса.
 *
 * @param data Структура для заполнения.
 * @param proc_count Общее кол-во процессов.
 *
 * @return true, если заполнение на данном кол-ве процессов возможно.
 */
bool fill_initial(calc_data_t* data, size_t proc_count) {
  // x0, y0, z0
  data->center.x = -1;
  data->center.y = -1;
  data->center.z = -1;

  // Dx, Dy, Dz
  data->distance.x = 2;
  data->distance.y = 2;
  data->distance.z = 2;

  // Nx, Ny, Nz
  data->disc_grid.x = 20;
  data->disc_grid.y = 20;
  data->disc_grid.z = 20;

  // h_x, h_y, h_z
  data->height.x = data->distance.x / (data->disc_grid.x - 1);
  data->height.y = data->distance.y / (data->disc_grid.y - 1);
  data->height.z = data->distance.z / (data->disc_grid.z - 1);

  data->paramA = pow(10, 5);
  data->epsilon = pow(10, -8);

  // \phi^0_{i, j, k}
  data->initial_approx = 0;

  // TODO - размеры локальной области
  data->area_size.x = 0;
  data->area_size.y = 0;
  data->area_size.z = 0;

  // TODO - выделение памяти и заполнение граничных областей
  data->current_area = (double*) calloc(data->area_size.x *
                                        data->area_size.y *
                                        data->area_size.z *
                                        sizeof(double));
  data->next_area = (double*) malloc(data->area_size.x *
                                     data->area_size.y *
                                     data->area_size.z *
                                     sizeof(double));
}

/**
 * Вычисляет значение функции ро в заданной точке.
 * Функция фи, от которой зависит ро, захардкожена.
 *
 * @param data Набор данных для вычисления.
 * @param i Позиция по X.
 * @param j Позиция по Y.
 * @param k Позиция по Z.
 *
 * @return Значение функции ро в точке.
 */
double calculate_ro(calc_data_t* data, size_t i, size_t j, size_t k) {
  double x = data->center.x + i * data->height.x;
  double y = data->center.y + j * data->height.y;
  double z = data->center.z + k * data->height.z;
  double phi = pow(x, 2) + pow(y, 2) + pow(z, 2);
  return 6 - a * phi;
}

/**
 * Выбирает значение функции из текущей области.
 *
 * @param data Таблица для выборки значения.
 * @param i Позиция по X.
 * @param j Позиция по Y.
 * @param k Позиция по Z.
 *
 * @return Значение по данной точке.
 */
double select(double* data, size_t i, size_t j, size_t k) {
  size_t height = (size_t) data->area_size.y;
  size_t depth = (size_t) data->area_size.z;
  return data[i * height * depth + j * depth + k];
}

/**
 * Вставляет новое значение функции в область следующего шага.
 *
 * @param data Таблица для вставки значения.
 * @param value Вставляемое значение.
 * @param i Позиция по X.
 * @param j Позиция по Y.
 * @param k Позиция по Z.
 */
void insert(double* data, double value, size_t i, size_t j, size_t k) {
  size_t height = (size_t) data->area_size.y;
  size_t depth = (size_t) data->area_size.z;
  data[i * height * depth + j * depth + k] = value;
}

/**
 * Вычисляет следующее значение функции в требуемой точке из текущей
 * области и записывает в новую область.
 *
 * @param data Набор данных для вычисления.
 * @param i Позиция по X.
 * @param j Позиция по Y.
 * @param k Позиция по Z.
 */
void calculate_next_phi_at(calc_data_t* data, size_t i, size_t j, size_t k) {
  double result;
  double powHx = pow(data->height.x, 2);
  double powHy = pow(data->height.y, 2);
  double powHz = pow(data->height.z, 2);

  double first =  select(data->current_area, i + 1, j, k) -
                  select(data->current_area, i, j, k) * 2 +
                  select(data->current_area, i - 1, j, k);

  double second = select(data->current_area, i, j + 1, k) -
                  select(data->current_area, i, j, k) * 2 +
                  select(data->current_area, i, j - 1, k);

  double third =  select(data->current_area, i, j, k + 1) -
                  select(data->current_area, i, j, k) * 2 +
                  select(data->current_area, i, j, k - 1);

  double divider = 2 / powHx + 2 / powHy + 2 / powHz + data->paramA;
  double ro = calculate_ro(data, i, j, k);

  first /= powHx;
  second /= powHy;
  third /= powHz;

  result = (first + second + third - ro) / divider;
  insert(data->next_area, result, i, j, k);
}

/**
 * Проверяет достижение порога сходимости.
 *
 * @param data Набор данных для вычисления.
 *
 * @return true, если процесс сошелся.
 */
bool check_finish(calc_data_t* data) {
  double max = 0;
  double value;

  // TODO - правильные смещения и размеры
  for (size_t i = 0; i < data->area_size.x; i++)
    for (size_t j = 0; j < data->area_size.y; j++)
      for (size_t k = 0; k < data->area_size.z; k++) {
        value = select(data->next_area, i, j, k) -
                select(data->current_area, i, j, k);
        value = abs(value);
        if (max > value)
          max = value;
      }

  return max < data->epsilon;
}

/**
 * Обсчитывает границы областей перед их отправкой.
 *
 * @param data Набор данных для вычислений.
 */
void calculate_borders(calc_data_t* data) {

}

/**
 * Обсчитывает центр области.
 *
 * @param data Набор данных для вычислений.
 */
void calculate_center(calc_data_t* data) {

}

/**
 * Отправляет граничные области после их обсчета.
 *
 * @param data Данные для отправки.
 */
void send_borders(calc_data_t* data) {

}

/**
 * Принимает граничные области от других процессов и обновляет области
 * для сдедующего итерацеонного шага.
 */
void prepare_next_step(calc_data_t* data) {

}

int main(int argc, char* argv[]) {
  calc_data_t data;
  fill_initial(&data, 0); // TODO - кол-во процессов

  bool next = true;

  while (next) {
    calculate_borders(&data);
    send_borders(&data);
    calculate_center(&data);
    next = !check_finish(&data);
    prepare_next_step(&data);
  }

  return 0;
}

