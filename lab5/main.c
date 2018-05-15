
#include <stdbool.h>

#include "communicator.h"
#include "executor.h"

static void* counting_thread(void* state);
static void* scanning_thread(void* state);
static void* awaiting_thread(void* state);


int main(int argc, char* argv[]) {
  return 0;
}

/**
 * Поток для вычисления задач.
 */
static void* counting_thread(void* state) {
  model_t* model = (model_t*) state;
  return NULL;
}

/**
 * Поток для поиска свободных задач.
 */
static void* scanning_thread(void* state) {
  model_t* model = (model_t*) state;
  return NULL;
}

/**
 * Поток ожидающий взаимодействие с процессом.
 */
static void* awaiting_thread(void* state) {
  model_t* model = (model_t*) state;
  int res;
  task_t task;
  while (true) {
    res = comm_wait_for_request(MPI_COMM_WORLD);
    if (res == -1)
      return NULL;
    if (model_steal_task(model, &task))
      comm_share_task(res, &task, MPI_COMM_WORLD);
    else
      comm_answer_no(res, MPI_COMM_WORLD);
  }
  return NULL;
}

