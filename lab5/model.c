
#include <stdlib.h>
#include <errno.h>

#include "model.h"

bool model_init(model_t* model,
                size_t tasks_num,
                generator_data_t data,
                task_t (*generator)(generator_data_t*)) {
  if (!model || !generator)
    return false;

  if (pthread_mutex_init(&model->tasks_mutex, NULL))
    return false;

  if (!(model->tasks = (task_t*) malloc(sizeof(task_t) * tasks_num))) {
    int error = errno;
    pthread_mutex_destroy(&model->tasks_mutex);
    errno = error;
    return false;
  }

  model->tasks_pos = tasks_num - 1;
  model->tasks_size = tasks_num;

  for (size_t i = 0; i < tasks_num; i++) {
    model->tasks[i] = generator(&data);
    data.iteration++;
  }

  return true;
}

void model_release(model_t* model) {
  if (!model || pthread_mutex_trylock(&model->tasks_mutex))
    return;

  pthread_mutex_destroy(&model->tasks_mutex);
  free(model->tasks);
}

bool model_steal_task(model_t* model, task_t* task) {
  if (!model)
    return false;

  if (pthread_mutex_lock(&model->tasks_mutex))
    return false;

  if (!model->tasks_pos) {
    pthread_mutex_unlock(&model->tasks_mutex);
    return false;
  }

  (*task) = model->tasks[model->tasks_pos--];

  pthread_mutex_unlock(&model->tasks_mutex);
  return true;
}

bool model_add_task(model_t* model, task_t* task) {
  if (!model)
    return false;

  if (pthread_mutex_lock(&model->tasks_mutex))
    return false;

  if (model->tasks_pos + 1 >= model->tasks_size) {
    pthread_mutex_unlock(&model->tasks_mutex);
    return false;
  }

  model->tasks[model->tasks_pos++] = (*task);

  pthread_mutex_unlock(&model->tasks_mutex);
  return true;
}

size_t model_tasks_count(model_t* model) {
  size_t value;
  if (!model)
    return 0;

  if (pthread_mutex_lock(&model->tasks_mutex))
    return 0;

  value = model->tasks_pos;

  pthread_mutex_unlock(&model->tasks_mutex);
  return value;
}

