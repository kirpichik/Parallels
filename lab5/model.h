
#ifndef _MODEL_H
#define _MODEL_H

#include <pthread.h>
#include <stdbool.h>

typedef struct generator_data {
  int iteration;
  int rank;
  int processes;
} generator_data_t;

typedef size_t task_t;

typedef struct model {
  pthread_mutex_t tasks_mutex;
  pthread_cond_t tasks_cond;
  volatile bool interrupted;
  size_t tasks_pos;
  size_t tasks_size;
  task_t* tasks;
} model_t;

/**
 * Инициализирует модель и заполняет первоначальный список задач при помощи
 * генератора задач.
 *
 * @param model Модель для инициализации.
 * @param tasks_num Количество задач, которые нужно сгенерировать.
 * @param data Начальные данные для генерации задач.
 * @param generator Генератор задач.
 *
 * @return true, если инициализация прошла успешно.
 */
bool model_init(model_t* model,
                size_t tasks_num,
                generator_data_t data,
                task_t (*generator)(generator_data_t*));

/**
 * Разблокирует кондишоны для прерывания потоков.
 *
 * @param model Модель для прерывания.
 */
void model_interrupt(model_t* model);

/**
 * Проверяет, была ли модель прервана.
 *
 * @param model Модель.
 *
 * @return true, если прерывание было вызвано.
 */
bool model_is_interrupted(model_t* model);

/**
 * Освобождает сохраненные данные в модели.
 *
 * @param model Модель для освобождения.
 */
void model_release(model_t* model);

/**
 * Забирает одну задачу из списка задач.
 * Если задач в списке нет, возвращает false.
 *
 * @param model Модель.
 * @param task Результат получения задачи.
 *
 * @return true, если задача была забрана.
 */
bool model_steal_task(model_t* model, task_t* task);

/**
 * Забирает одну задачу из списка задач.
 * Если задач в списке нет,
 * блокирует выполнение потока до появления задачи.
 *
 * @param model Модель.
 * @param task Результат получения задачи.
 *
 * @return true, если задача была забрана и false, если взятие прервано.
 */
bool model_steal_task_await(model_t* model, task_t* task);

/**
 * Добавляет задачу в список задач.
 * Если места в списке нет, возвращает false.
 *
 * @param model Модель.
 * @param task Добавляемая задача.
 *
 * @return true, если задача была добавлена и false, если добавление прервано.
 */
bool model_add_task(model_t* model, task_t* task);

/**
 * Получает количество доступных задач.
 *
 * @param model Модель.
 *
 * @return Количество доступных задач.
 */
size_t model_tasks_count(model_t* model);

#endif

