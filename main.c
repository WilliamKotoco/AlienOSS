#include "ui/main-window.h"
#include "process/process.h"

/// creating global variables 
List *PCB;
Memory *memory;
int processes_id;


int main(int argc, char *argv[]) {
  /// inicializing global variables
  PCB = create_list(sizeof(Process), compare_processes); 

  memory = malloc(sizeof(Memory));
  memory->pages = create_list(sizeof(Page), compare_pages);
  memory->num_free_pages = NUM_PAGES; //!< at first, all pages (beside the ones used by the OS) are free

  memory->semaphores = create_list(sizeof(Semaphore), compare_semaphores);

  //show_and_run();

  create_process("test/programa_sintetico.txt");

  return 0;
}
