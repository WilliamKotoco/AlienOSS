#include "scheduler/scheduler.h"
#include "../process/process.h"

extern Scheduler *scheduler;
extern sem_t process_semaphore;

void init_scheduler() {
  scheduler = malloc(sizeof(Scheduler));
  scheduler->running_process = NULL; /// set the current process to NULL

  /// @TEMP: this will probaly causes circular dependencies, i'm sure
  /// process will have to import scheduler.h
  scheduler->ready_processes = create_list(sizeof(Process), compare_processes);
}

void forward_scheduling() {
  sem_wait(&process_semaphore); /// we cannot change the ready processes list in
                                /// between these following actions

  Node *scheduled = pop(scheduler->ready_processes); /// first process on the queue
  if(! scheduled){ /// there is no process on the list
    return; 
  }

  Process *scheduled_process = (Process *) scheduled->data;

  scheduled_process->status = RUNNING;
  scheduled_process->remaining_time = QUANTUM_TIME_TOTAL / scheduled_process->priority; /// process's quantum time is inversely proportional to its priority  
    
  // OBS FALTA TRATAR O PROCESSO "REMOVIDO" (SE VOLTA PARA A LISTA COMO PRONTO OU SE ESTÁ BLOQUEADO)

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

  if (tmp_process->priority >
      next_priority) { /// new process has the highest priority
    return NULL;       // there is no process before it
  }

  while (tmp) { // searches for the first process with next_priority
    if (tmp_process->priority == next_priority)
      return tmp->prev; // the last process with priority

    tmp = tmp->next;
    tmp_process = (Process *)tmp->data;
  }

  return list->tail;
}

void add_process_scheduler(Process *new_process) {
  Node *prev =
      last_process_priority(scheduler->ready_processes, new_process->priority);

  Node *new_node = malloc(sizeof(Node));
  new_node->data = new_process;
  new_node->prev = new_node->next = NULL;

  if (!prev) { // new_process has the highest priority and is the head
    new_node->next = scheduler->ready_processes->header;
    scheduler->ready_processes->header = new_node;
  } else { // new_process is added after prev
    new_node->next = prev->next;
    prev->next = new_node;

    if (!new_node->next) { /// new process is the last node (the tail)
      scheduler->ready_processes->tail = new_node;
      new_node->next = scheduler->ready_processes->header;
    }
  }
}