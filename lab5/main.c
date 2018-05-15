#include <stdio.h>

/**
 * Поток для вычисления задач.
 */
void* counting_thread(void* state);

/**
 * Поток для поиска свободных задач.
 */
void* scanning_thread(void* state);

/**
 * Поток ожидающий взаимодействие с процессом.
 */
void* awaiting_thread(void* state);

int main(int argc, char* argv[]) {
  printf("Lab 5\n");
  return 0;
}

void* counting_thread(void* state) {
  return NULL;
}

void* scanning_thread(void* state) {
  return NULL;
}

void* awaiting_thread(void* state) {
  return NULL;
}

