#include "scheduler/scheduler.h"
#include "../process/process.h"
extern Scheduler *scheduler;
extern sem_t process_semaphore;

void init_scheduler() {
  scheduler = malloc(sizeof(Scheduler));
  scheduler->ready_processes = NULL; /// set the current process to NULL

  /// @TEMP: this will probaly causes circular dependencies, i'm sure
  /// process will have to import scheduler.h
  scheduler->ready_processes = create_list(sizeof(Process), compare_processes);
}

void forward_scheduling() {
  sem_wait(&process_semaphore);

  /// codigo aqui

  sem_post(&process_semaphore);
}