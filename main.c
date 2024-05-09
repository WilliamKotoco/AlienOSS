#include "ui/main-window.h"
#include "process/process.h"

List *PCB;
Memory *memory;

int main(int argc, char *argv[]) {
  // inicializar coisas, como listas, etc
  PCB = create_list(sizeof(Process), compare_processes);
  memory->pages = create_list(sizeof(Page), compare_pages);

  show_and_run();

  return 0;
}
