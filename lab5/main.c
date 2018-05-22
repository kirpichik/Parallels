
#include <stdbool.h>
#include <pthread.h>
#include <mpi.h>
#include <stdio.h>

#include "communicator.h"
#include "executor.h"

#ifdef DEBUG_LEVEL
#define DEBUG_PRINT(msg, rank) printf("%d:%s: %s\n", rank, __func__, msg);
#else
#define DEBUG_PRINT(msg, rank) ;
#endif

#define TASKS_NUM 10000

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
  if (provided_level != MPI_THREAD_MULTIPLE) {
    fprintf(stderr, "ERROR: In this version there is not \
implemented a required level of multithreading!\n");
    return -1;
  }
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
    DEBUG_PRINT("Awaiting for empty", model->data.rank);
    if (!model_await_for_empty(model))
      continue;

    found = false;
    for (int i = 0; i < model->data.processes; i++) {
      if (i == model->data.rank)
        continue;

      DEBUG_PRINT("Sending request for task", model->data.rank);
      if (comm_send_request_task(model->data.rank, i, &task, MPI_COMM_WORLD)) {
        DEBUG_PRINT("Received task", model->data.rank);
        model_add_task(model, &task);
        DEBUG_PRINT("Task added", model->data.rank);
        found = true;
        break;
      }
    }

    DEBUG_PRINT("No free tasks", model->data.rank);
    // Не найдено задач, рассылаем всем завершение
    if (!found) {
      model_interrupt(model);
      for (int i = 0; i < model->data.processes; i++)
        comm_finish(i, MPI_COMM_WORLD);
    }
    DEBUG_PRINT("Finishes sent", model->data.rank);
  }
  return NULL;
}

/**
 * Поток для вычисления задач.
 */
static void* counting_thread(void* state) {
  model_t* model = (model_t*) state;
  task_t task;
  size_t count = 0;;

  while (!model_is_interrupted(model)) {
    DEBUG_PRINT("Stealing task", model->data.rank);
    if (model_steal_task_await(model, &task)) {
      DEBUG_PRINT("Executing task", model->data.rank);
      executor_exec_task(&task);
      count++;
    }
  }
#ifdef DEBUG_LEVEL
   printf("%d:%s: tasks counted: %lu\n", model->data.rank, __func__, count);
#endif

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
    DEBUG_PRINT("Waiting for input", model->data.rank);
    if ((res = comm_wait_for_request(MPI_COMM_WORLD)) == -1) {
      DEBUG_PRINT("Received finish", model->data.rank);
      received_finish++;
      continue;
    }

    DEBUG_PRINT("Received request", model->data.rank);
    if (model_steal_task(model, &task)) {
      DEBUG_PRINT("Task sent", model->data.rank);
      comm_share_task(res, &task, MPI_COMM_WORLD);
    }
    else {
      DEBUG_PRINT("No tasks", model->data.rank);
      comm_answer_no(res, MPI_COMM_WORLD);
    }
  }
  return NULL;
}

