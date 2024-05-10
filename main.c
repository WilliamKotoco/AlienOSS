#include "process/process.h"
#include "ui/main-window.h"

/// creating global variables
List *PCB;
Memory *memory;
int processes_id;

int main(int argc, char *argv[]) {
  /// inicializing global variables
  PCB = create_list(sizeof(Process), compare_processes);

  memory = malloc(sizeof(Memory));
  memory->pages = create_list(sizeof(Page), compare_pages);
  memory->num_free_pages = NUM_PAGES; //!< at first, all pages (beside the ones
                                      //!< used by the OS) are free

  memory->semaphores = create_list(sizeof(Semaphore), compare_semaphores);

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
