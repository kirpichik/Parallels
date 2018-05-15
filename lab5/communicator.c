
#include <stdint.h>

#include "communicator.h"

#define HEAD_PACKET_LEN 1
#define RANK_PACKET_LEN 2
#define TASK_PACKET_LEN 8

#define TAG_CLIENT          123
#define PACKET_REQUEST_TASK 0
#define PACKET_FINISH       1

#define TAG_SERVER          321
#define PACKET_NO_TASKS     2
#define PACKET_SHARE_TASK   3

bool comm_send_request_task(size_t self_rank, size_t receiver, task_t* task, MPI_Comm comm) {
  unsigned char send_buff[HEAD_PACKET_LEN + RANK_PACKET_LEN];
  unsigned char recv_buff[HEAD_PACKET_LEN + TASK_PACKET_LEN];
  send_buff[0] = PACKET_REQUEST_TASK;
  (*(send_buff + 1)) = (uint16_t) self_rank;

  MPI_Send(send_buff, sizeof(send_buff), MPI_CHAR, receiver, TAG_CLIENT, comm);

  MPI_Recv(recv_buff, sizeof(recv_buff), MPI_CHAR, receiver, TAG_SERVER, comm, MPI_STATUS_IGNORE);

  if (recv_buff[0] == PACKET_SHARE_TASK)
    (*task) = (task_t) (*(recv_buff + 1));

  return recv_buff[0] == PACKET_SHARE_TASK;
}

void comm_finish(size_t self_rank, MPI_Comm comm) {
  unsigned char buff[HEAD_PACKET_LEN + RANK_PACKET_LEN];
  buff[0] = PACKET_FINISH;
  (*(buff + 1)) = (uint16_t) self_rank;

  MPI_Send(buff, sizeof(buff), MPI_CHAR, self_rank, TAG_CLIENT, comm);
}

int comm_wait_for_request(MPI_Comm comm) {
  unsigned char buff[HEAD_PACKET_LEN + RANK_PACKET_LEN];

  MPI_Recv(buff, sizeof(buff), MPI_CHAR, MPI_ANY_SOURCE, TAG_CLIENT, comm, MPI_STATUS_IGNORE);

  if (buff[0] == PACKET_FINISH)
    return -1;

  return (uint16_t) (*(buff + 1));
}

void comm_share_task(size_t receiver, task_t* task, MPI_Comm comm) {
  unsigned char buff[HEAD_PACKET_LEN + TASK_PACKET_LEN];
  buff[0] = PACKET_SHARE_TASK;
  (*(buff + 1)) = (uint64_t) task;

  MPI_Send(buff, sizeof(buff), MPI_CHAR, receiver, TAG_SERVER, comm);
}

void comm_answer_no(size_t receiver, MPI_Comm comm) {
  unsigned char buff[HEAD_PACKET_LEN + TASK_PACKET_LEN];
  buff[0] = PACKET_NO_TASKS;

  MPI_Send(buff, sizeof(buff), MPI_CHAR, receiver, TAG_SERVER, comm);
}

