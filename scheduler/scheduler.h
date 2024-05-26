#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../log/log.h"
#include "../process/process.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define QUANTUM_TIME_TOTAL 5000

typedef struct scheduler {
  Process *running_process;
  List *ready_processes;
} Scheduler;

///  @brief Initializes the scheduler, setting the fields to NULL
void init_scheduler();

///  @brief schedules the next process on the queue
///  @details chooses the oldest process with the highest priority to run (if
/// there is any), changing its status to RUNNING and calculating its quantum
/// time
void forward_scheduling();

///  @brief Adds a process to the scheduler's ready process's list
///  @param new_process the process to be added
///  @details Given the new process and the scheduler's list, adds the process
/// in a priority queue manner
void add_process_scheduler(Process *new_process);
#endif