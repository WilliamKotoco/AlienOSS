#include "cpu.h"
#include "../scheduler/scheduler.h"

/// Necessary for global variables
extern Scheduler *scheduler;
extern sem_t process_semaphore;

void init_cpu() {
  pthread_t cpu_id;

  pthread_attr_t cpu_attr;

  pthread_attr_init(&cpu_attr);

  pthread_attr_setscope(&cpu_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&cpu_id, NULL, (void *)execute, NULL);
}

void cpu() {
  while (1) {
    while (scheduler->running_process && scheduler->running_process->PC < 40 &&
           scheduler->running_process->time) {
      sem_wait(&process_semaphore);

      /// if(instrucoes do processo nao estao carregadas)
      ///        carregar

      /// executa instrucao
      /// scheduler->pc++;

      sem_post(&process_semaphore);
    }
  }
}