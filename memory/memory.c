#include "memory.h"
/// Necessary for using global variables defined in main.c
extern Memory *memory;

int compare_pages(void *d1, void *d2) {
  Page *p1 = (Page *)d1;

  int d11 = p1->number;

  int *d22 = (int *)d2;

  if (d11 == *d22) {
    return 1;
  } else {
    return 0;
  }
}

Memory *init_memory(){
  Memory *memory = malloc(sizeof(Memory));

  memory->pages = malloc(sizeof(Page) * NUM_PAGES);
  memory->num_free_pages = NUM_PAGES; //!< at first, all pages (beside the ones
                                      //!< used by the OS) are free

  for(int i = 0; i < NUM_PAGES; i++){
    memory->pages[i].number = i;
    memory->pages[i].free = 1;
  }

  memory->semaphores = create_list(sizeof(Semaphore), compare_semaphores);

  return memory;
}