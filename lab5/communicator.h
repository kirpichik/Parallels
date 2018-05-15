
#ifndef _COMMUNICATOR_H
#define _COMMUNICATOR_H

#include <stdbool.h>
#include <mpi.h>

#include "model.h"

/**
 * Посылает запрос на переброс задачи указанному процессу.
 *
 * @param self_rank Ранг данного процесса.
 * @param receiver Целевой процесс.
 * @param task Буфер задачи.
 * @param comm Коммуникатор процессов.
 *
 * @return true, если задача переброшена.
 */
bool comm_send_request_task(size_t self_rank, size_t receiver, task_t* task, MPI_Comm comm);

/**
 * Завершает блокирующий вызов ожидания входящих пакетов.
 *
 * @param self_rank Ранг данного процесса.
 * @param comm Коммуникатор процессов.
 */
void comm_finish(size_t self_rank, MPI_Comm comm);

/**
 * Блокируется на ожидании входящего запроса на переброс задачи.
 *
 * @param comm Коммуникатор процессов.
 *
 * @return Ранг процесса, запросившего задачу, либо -1, если прием завершен.
 */
int comm_wait_for_request(MPI_Comm comm);

/**
 * Отправляет задачу указанному процессу.
 *
 * @param receiver Целевой процесс.
 * @param task Посылаемая задача.
 * @param comm Коммуникатор процессов.
 */
void comm_share_task(size_t receiver, task_t* task, MPI_Comm comm);

/**
 * Отправляет ответ указанному процессу, что свободных задач нет.
 *
 * @param receiver Целевой процесс.
 * @param comm Коммуникатор процессов.
 */
void comm_answer_no(size_t receiver, MPI_Comm comm);

#endif

