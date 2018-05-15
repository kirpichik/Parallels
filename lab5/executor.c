
#include <math.h>
#include <stdlib.h>

#include "executor.h"

task_t executor_generate_task(generator_data_t* data) {
  return abs(data->rank - (data->iteration % data->processes));
}

result_t executor_exec_task(task_t* task) {
  size_t num = (*task);
  double result = 0;
  for (size_t i = 0; i < num; i++)
    result += sqrt(i);
  return result;
}

