
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

  if (pthread_cond_init(&model->tasks_cond, NULL)) {
    int error = errno;
    pthread_mutex_destroy(&model->tasks_mutex);
    errno = error;
    return false;
  }

  if (!(model->tasks = (task_t*) malloc(sizeof(task_t) * tasks_num))) {
    int error = errno;
    pthread_mutex_destroy(&model->tasks_mutex);
    pthread_cond_destroy(&model->tasks_cond);
    errno = error;
    return false;
  }

  model->data = data;
  model->tasks_pos = tasks_num - 1;
  model->tasks_size = tasks_num;
  model->interrupted = false;

  for (size_t i = 0; i < tasks_num; i++) {
    model->tasks[i] = generator(&data);
    data.iteration++;
  }

  return true;
}

void model_interrupt(model_t* model) {
  if (!model)
    return;
  model->interrupted = true;
  pthread_cond_signal(&model->tasks_cond);
}

bool model_is_interrupted(model_t* model) {
  return model && model->interrupted;
}

void model_release(model_t* model) {
  if (!model || pthread_mutex_trylock(&model->tasks_mutex))
    return;

  pthread_mutex_destroy(&model->tasks_mutex);
  pthread_cond_destroy(&model->tasks_cond);
  free(model->tasks);
}

bool model_steal_task(model_t* model, task_t* task) {
  if (!model || pthread_mutex_lock(&model->tasks_mutex))
    return false;

  if (!model->tasks_pos) {
    pthread_mutex_unlock(&model->tasks_mutex);
    return false;
  }

  (*task) = model->tasks[model->tasks_pos--];
  pthread_cond_signal(&model->tasks_cond);

  pthread_mutex_unlock(&model->tasks_mutex);
  return true;
}

bool model_steal_task_await(model_t* model, task_t* task) {
  if (!model || model->interrupted || pthread_mutex_lock(&model->tasks_mutex))
    return false;

  while (!model->tasks_pos) {
    pthread_cond_wait(&model->tasks_cond, &model->tasks_mutex);
    if (model->interrupted) {
      pthread_mutex_unlock(&model->tasks_mutex);
      return false;
    }
  }

  (*task) = model->tasks[model->tasks_pos--];
  pthread_cond_signal(&model->tasks_cond);

  pthread_mutex_unlock(&model->tasks_mutex);
  return true;
}

bool model_await_for_empty(model_t* model) {
  if (!model || pthread_mutex_lock(&model->tasks_mutex))
    return false;

  while (model->tasks_pos) {
    pthread_cond_wait(&model->tasks_cond, &model->tasks_mutex);
    if (model->interrupted) {
      pthread_mutex_unlock(&model->tasks_mutex);
      return false;
    }
  }

  pthread_mutex_unlock(&model->tasks_mutex);

  return true;
}

bool model_add_task(model_t* model, task_t* task) {
  if (!model || pthread_mutex_lock(&model->tasks_mutex))
    return false;

  while (model->tasks_pos + 1 >= model->tasks_size) {
    pthread_cond_wait(&model->tasks_cond, &model->tasks_mutex);
    if (model->interrupted) {
      pthread_mutex_unlock(&model->tasks_mutex);
      return false;
    }
  }

  model->tasks[model->tasks_pos++] = (*task);
  pthread_cond_signal(&model->tasks_cond);

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

