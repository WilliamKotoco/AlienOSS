#include "scheduler.h"
#include <semaphore.h>

extern Scheduler *scheduler;
extern sem_t process_semaphore;

void init_scheduler() {
  scheduler = malloc(sizeof(Scheduler));
  scheduler->running_process = NULL; /// set the current process to NULL

  scheduler->ready_processes = create_list(sizeof(Process), compare_processes);
}

void forward_scheduling() {
  /// ensures that a new process does not interrupt a running process in the
  /// middle of an instruction
  sem_wait(&process_semaphore);

  Process *old_process = scheduler->running_process;

  /// if the process is still ready, it is added again in the list
  if (old_process && old_process->status == READY) {
    add_process_scheduler(old_process);
  }

  /// schedules first process on the queue
  Node *scheduled = pop(scheduler->ready_processes);

  /// there isn't a process to schedule
  if (!scheduled) {
    /// frees the current running process since it won't be replaced
    free(scheduler->running_process);
    scheduler->running_process = NULL;

    sem_post(&process_semaphore);

    return;
  }

  /// casting of the gerenic node
  Process *scheduled_process = (Process *)scheduled->data;

  scheduled_process->status = RUNNING;

  /// process' quantum time is inversely proportional to its priority
  scheduled_process->remaining_time =
      QUANTUM_TIME_TOTAL / scheduled_process->priority;

  scheduler->running_process = scheduled_process;

  sem_post(&process_semaphore);
}

Node *last_process_priority(List *list, int priority) {
  Node *tmp = list->header;

  if (!tmp) { // list is empty
    return NULL;
  }

  Process *tmp_process = (Process *)tmp->data;
  int next_priority = priority + 1;

  /// new process has the highest priority
  if (tmp_process->priority > next_priority) {
    return NULL; // there is no process before it
  }

  while (tmp) { // searches for the first process with next_priority
    tmp_process = (Process *)tmp->data;

    if (tmp_process->priority == next_priority)
      return tmp->prev; // the last process with priority

    tmp = tmp->next;
  }

  return list->tail;
}

void add_process_scheduler(Process *new_process) {
  /// previous process to new process, ordering by priority
  Node *prev =
      last_process_priority(scheduler->ready_processes, new_process->priority);

  /// alocates space
  Node *new_node = malloc(sizeof(Node));
  new_node->data = new_process;
  new_node->prev = new_node->next = NULL;

  if (!prev) { /// new_process has the highest priority and is the head
    new_node->next = scheduler->ready_processes->header;
    scheduler->ready_processes->header = new_node;
  } else { /// new_process is added after prev
    new_node->next = prev->next;
    prev->next = new_node;

    if (!new_node->next) { /// new process is the last node (the tail)
      scheduler->ready_processes->tail = new_node;
      new_node->next = scheduler->ready_processes->header;
    }
  }
}
