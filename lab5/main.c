
#include <stdbool.h>
#include <pthread.h>
#include <mpi.h>

#include "communicator.h"
#include "executor.h"

#define TASKS_NUM 100

static void* counting_thread(void* state);
static void* scanning_thread(void* state);
static void* awaiting_thread(void* state);

int main(int argc, char* argv[]) {
  model_t model;
  generator_data_t data;
  int provided_level;
  void* result;

  pthread_attr_t attr;
  pthread_t thread_scanning;
  pthread_t thread_counting;
  pthread_t thread_awaiting;

  // Инициализация общих данных
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided_level);
  MPI_Comm_size(MPI_COMM_WORLD, &data.processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &data.rank);
  data.iteration = 0;
  model_init(&model, TASKS_NUM, data, &executor_generate_task);

  // Инициализация и запуск потоков
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_create(&thread_scanning, &attr, &scanning_thread, &model);
  pthread_create(&thread_counting, &attr, &counting_thread, &model);
  pthread_create(&thread_awaiting, &attr, &awaiting_thread, &model);

  // Ожидание завершения потоков
  pthread_join(thread_scanning, &result);
  pthread_join(thread_counting, &result);
  pthread_join(thread_awaiting, &result);

  // Освобождение ресурсов
  pthread_attr_destroy(&attr);
  model_release(&model);
  MPI_Finalize();
  return 0;
}

/**
 * Поток для поиска свободных задач.
 */
static void* scanning_thread(void* state) {
  model_t* model = (model_t*) state;
  task_t task;
  bool found;

  while (!model_is_interrupted(model)) {
    if (!model_await_for_empty(model))
      continue;

    found = false;
    for (int i = 0; i < model->data.processes; i++) {
      if (i == model->data.rank)
        continue;

      if (comm_send_request_task(model->data.rank, i, &task, MPI_COMM_WORLD)) {
        model_add_task(model, &task);
        found = true;
        break;
      }
    }

    // Не найдено задач, рассылаем всем завершение
    if (!found) {
      model_interrupt(model);
      for (int i = 0; i < model->data.processes; i++)
        comm_finish(i, MPI_COMM_WORLD);
    }
  }
  return NULL;
}

/**
 * Поток для вычисления задач.
 */
static void* counting_thread(void* state) {
  model_t* model = (model_t*) state;
  task_t task;

  while (!model_is_interrupted(model)) {
    if (model_steal_task_await(model, &task))
      executor_exec_task(&task);
  }
  return NULL;
}

/**
 * Поток ожидающий взаимодействие с процессом.
 */
static void* awaiting_thread(void* state) {
  model_t* model = (model_t*) state;
  int res;
  task_t task;
  size_t received_finish = 0;

  while (received_finish < model->data.processes) {
    if ((res = comm_wait_for_request(MPI_COMM_WORLD)) == -1) {
      received_finish++;
      continue;
    }

    if (model_steal_task(model, &task))
      comm_share_task(res, &task, MPI_COMM_WORLD);
    else
      comm_answer_no(res, MPI_COMM_WORLD);
  }
  return NULL;
}

