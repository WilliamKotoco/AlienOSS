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

int compare_segments(void *d1, void *d2) {
  Segment *s1 = (Segment *)d1;

  int s11 = s1->id;

  int *s22 = (int *)d2;
  int s23 = *s22;

  if (s11 == s23) {
    return 1;
  } else {
    return 0;
  }
}

Memory *init_memory() {
  Memory *memory = malloc(sizeof(Memory));

  memory->pages = malloc(sizeof(Page) * NUM_PAGES);
  memory->num_free_pages = NUM_PAGES; //!< at first, all pages (beside the ones
                                      //!< used by the OS) are free

  for (int i = 0; i < NUM_PAGES; i++) {
    memory->pages[i].number = i;
    memory->pages[i].free = 1;
  }

  memory->segments = create_list(sizeof(Segment), compare_segments);
  memory->next_swapped = NULL;

  memory->semaphores = create_list(sizeof(Semaphore), compare_semaphores);

  return memory;
}

void memory_load_requisition(Process *process) {
  /// not enough space, swapping needed
  while (memory->num_free_pages < process->segment->num_pages) {
    swap_segment();
  }

  /// there is enough space available in the memory
  load_segment(process);

  /// inserts segment in the segments list
  push(memory->segments, process->segment);
}

void load_segment(Process *process) {
  /// creates the pages and inserts them into memory's page table
  for (int i = 0; i < process->segment->num_pages; i++) {
    Page *new_page = malloc(sizeof(Page));

    new_page->process_id = process->id;
    new_page->segment_id = process->segment->id;

    add_page_memory(new_page);
  }
}

void add_page_memory(Page *new_page) {
  int i = 0;

  /// searches first free page
  while (memory->pages[i].free == 1 && i < NUM_PAGES) {
    i++;
  }

  /// loads page into memory
  memory->pages[i].number = i;
  memory->pages[i].process_id = new_page->process_id;
  memory->pages[i].segment_id = new_page->segment_id;
  memory->pages[i].free = 0;

  memory->num_free_pages--;
}

void memory_unload_segment(Segment *segment) {
  for (int i = 0; i < segment->num_pages; i++) {
    memory_delete_page(segment->id);
  }

  segment->dirty_bit = 0;
  segment->present_bit = 0;
  segment->used_bit = 0;

  /// remove segment from the list, as it is not loaded anymore
  delete_list(memory->segments, &segment->id);
}

void memory_delete_page(int id) {
  int i = 0;

  /// searches for the first used page of the process
  while (i < NUM_PAGES) {
    if (memory->pages[i].free == 0 && memory->pages[i].segment_id == id) {
      break;
    }
    i++;
  }

  /// frees page
  memory->pages[i].free = 1;
  memory->pages[i].process_id = -1;
  memory->pages[i].segment_id = -1;

  memory->num_free_pages++;
}

void swap_segment() {
  Node *tmp = memory->next_swapped == NULL ? memory->segments->header
                                           : memory->next_swapped;

  /// circular search in the list
  while (tmp) {
    Segment *tmp_seg = (Segment *)tmp->data;

    /// if the segment has not been used, it is unloaded
    if (tmp_seg->used_bit == 0) {
      memory_unload_segment(tmp_seg);

      // sleep(1);
      // char message[256];
      // snprintf(message, sizeof(message), "Swapping tirou %d", tmp_seg->id);
      // append_log_message(message, MEMORY_LOG);

      return;
    } else {
      /// gives a used segment a second chance
      tmp_seg->used_bit = 0;
    }

    /// last node
    if (!tmp->next) {
      /// makes the search ciruclar
      tmp = memory->segments->header;
    } else {
      tmp = tmp->next;
    }
  }

  /// uploades the next segment to be unloaded
  memory->next_swapped = tmp->next;
}