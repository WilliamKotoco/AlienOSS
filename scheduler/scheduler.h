#include "../process/process.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define QUANTUM_TIME_TOTAL 5000

typedef struct scheduler {
  Process *running_process;
  List *ready_processes;
} Scheduler;

/// Initializes the scheduler, setting the fiels to NULL
void init_scheduler();

void forward_scheduling();