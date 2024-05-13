#include "process/process.h"
#include "scheduler/scheduler.h"
#include "ui/main-window.h"
#include "cpu/cpu.h"
#include <semaphore.h>

/// creating global variables
List *PCB;
Memory *memory;
Scheduler *scheduler;
sem_t process_semaphore; /// Read-write semaphore for the current running
                         /// process in the scheduler.

int main(int argc, char *argv[]) {

  /// inicializing global variables
  sem_init(&process_semaphore, 1, 0);

  PCB = create_list(sizeof(Process), compare_processes);

  memory = init_memory();
  init_scheduler();
  init_cpu();
  show_and_run();

  // create_process("test/programa_sintetico.txt");

  // printf("Após ler programa sintético:\n");

  // Process *teste = (Process *)PCB->header->data;
  // printf("Cabeça da lista de processos: ID %d Name %s\n", teste->id,
  // teste->name);

  // Semaphore *sema = (Semaphore *)memory->semaphores->header->data;
  // printf("Semáforo: 1) %c ", sema->name);

  // Semaphore *sema2 = (Semaphore *)memory->semaphores->header->next->data;
  // printf("2) %c ", sema2->name);

  return 0;
}
