#include "scheduler.h"
#include <semaphore.h>

extern Scheduler *scheduler;
extern sem_t process_semaphore;
extern sem_t scheduler_semaphore;

void init_scheduler() {
  scheduler = malloc(sizeof(Scheduler));
  scheduler->running_process = NULL; /// set the current process to NULL

  scheduler->ready_processes = create_list(sizeof(Process), compare_processes);
}

void forward_scheduling() {
  sem_wait(
      &scheduler_semaphore); // necessário para alterar a lista de ready process

  Process *old_process = scheduler->running_process;

  /// if the process is still ready, it is added again in the list
  if (old_process && old_process->status == READY) {
    add_process_scheduler(old_process);
  }

  /// schedules first process on the queue
  Node *scheduled = pop(scheduler->ready_processes);

  /// there isn't a process to schedule
  if (!scheduled) {
    /// frees the current running process since there isn't a scheduled process
//    free(scheduler->running_process);
    scheduler->running_process = NULL;
    
    sem_post(&scheduler_semaphore);
    return;
  }

  /// casting of the gerenic node
  Process *scheduled_process = (Process *)scheduled->data;

  scheduled_process->status = RUNNING;

  /// process' quantum time is inversely proportional to its priority
  scheduled_process->remaining_time =
      QUANTUM_TIME_TOTAL / scheduled_process->priority;

  scheduler->running_process = scheduled_process;

  print_scheduled(scheduled_process);

  sem_post(&scheduler_semaphore);
}

void add_process_scheduler(Process *new_process) {
  /// alocates space
  Node *new_node = malloc(sizeof(Node));
  new_node->data = new_process;
  new_node->prev = new_node->next = NULL;

  /// empty list
  if (!scheduler->ready_processes->header) {
    scheduler->ready_processes->header = new_node;
    scheduler->ready_processes->tail = new_node;

    return;
  }

  Process *header = (Process *)scheduler->ready_processes->header->data;

  /// new process has the biggest priority
  if (new_process->priority < header->priority) {
    new_node->next = scheduler->ready_processes->header;
    scheduler->ready_processes->header = new_node;

    return;
  }

  /// search for the next process with a smaller priority
  Node *aux = scheduler->ready_processes->header;
  Process *next_process;

  while (aux->next) {
    next_process = (Process *)aux->next->data;

    /// first process with a bigger priority number (smaller priority overall)
    if (next_process->priority > new_process->priority) {
      break;
    }

    aux = aux->next;
  }

  new_node->next = aux->next;
  new_node->prev = aux;

  aux->next = new_node;

  if (!new_node->next) { /// new node is the last
    scheduler->ready_processes->tail = new_node;
  } else {
    aux->next->prev = new_node;
  }
}
