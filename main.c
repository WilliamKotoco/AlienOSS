#include "ui/main-window.h"
#include <semaphore.h>

#include <time.h>
/// creating global variables
List *PCB;
Memory *memory;
Scheduler *scheduler;
sem_t process_semaphore; /// Read-write semaphore for the current running
                         /// process in the scheduler.
bool new_process;
sem_t log_semaphore; /// semaphore responsible for controlling the logging.
List *LOGS;
int processes_id = 0;

int main(int argc, char *argv[]) {

  /// inicializing global variables
  sem_init(&process_semaphore, 1, 1);
  sem_init(&log_semaphore, 1, 0);
  new_process = false;

  PCB = create_list(sizeof(Process), compare_processes);
  LOGS = create_list(sizeof(LogMessage), compare_log);

  memory = init_memory();

  init_scheduler();
  init_cpu();
  show_and_run();

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
