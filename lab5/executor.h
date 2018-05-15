
#ifndef _EXECUTOR_H
#define _EXECUTOR_H

#include "model.h"

typedef double result_t;

/**
 * Генерирует входные данные задачи.
 *
 * @param data Данные для генерации задачи.
 *
 * @return Задача.
 */
task_t executor_generate_task(generator_data_t* data);

/**
 * Выполняет задачу.
 *
 * @param task Задача для исполнения.
 *
 * @return Результат выполнения задачи.
 */
result_t executor_exec_task(task_t* task);

#endif

