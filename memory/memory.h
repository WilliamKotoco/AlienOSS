#ifndef MEMORY_H
#define MEMORY_H

typedef struct process Process;

#include "../process/instruction.h"
#include "../process/process.h"
#include "../semaphore/semaphore.h"
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 1073742000
#define PAGE_SIZE 8192
#define NUM_PAGES 131072

#define KBYTE 1024

#define INSTR_SIZE 64
#define INSTR_PER_PAGE 128

/// Represents a memory page, identified by its number and that references the
/// segment and process it belongs to. A tuple on the memory's page table.
typedef struct page {
  int number;     //!< page number, identifier
  int segment_id; //!< reference to the segment this page belongs to
  int process_id; //!< reference to the process that owns the segment/page
  int free;       //!< indicates whether the page is free
} Page;

/// Represents a process segment, with its size, pages and instructions.
typedef struct segment {
  int id;          //!< segment identifier
  int size;        //!< size of the program data
  int num_pages;   ///< number of memory pages the segment occupies, its size /
                   ///< PAGE_SIZE
  int present_bit; //!< flag that indicates whether the segment is in memory
  Instruction *instructions; //!< array of the process instructions
  int num_instructions;      //!< number of instructions the program posesses
  long exec;

  int dirty_bit; //!< indicates whether the data has been modified since it came
                 //!< to memory
  int used_bit;  //!< indicates if the data has been read or written
} Segment;

/// Represents the memory of the system, contains the page table, needed for the
/// OS management.
typedef struct memory {
  Page *pages;        ///< memory table, array of its pages
  int num_free_pages; //!< number of free pages, NUM_PAGES - List *pages length

  List *segments; //!< segments used for swapping
  Node *next_swapped;

  List *semaphores; //!< semaphores managed by the OS
} Memory;

/// @brief Compares two pages based on their number. Used on the generic list.
/// @param d1 a page
/// @param d2 the id of page being compared to d1
/// @return 1 if d1 and d2 are the same page, and 0 otherwise
int compare_pages(void *d1, void *d2);

/// @brief Initializes the system's memory
/// @return the memory structure initialized
Memory *init_memory();

///  @brief Executes a memory load operation
///  @param process the process requisiting the operation
///  @details Creates the pages of the process's segment and inserts them into
/// memory
void memory_load_requisition(Process *);

///  @brief Loads a process' segment into memory
///  @param process given process
void load_segment(Process *);

///  @brief Adds a page into the memory's page table
///  @details Inserts a new page into the memory page table and performs the
/// swapping if there aren't enough pages free
void add_page_memory(Page *new_page);

///  @brief Unloads a process' segment
///  @param segment segment to be unloaded
///  @details removes each of the segment's pages and updates the segment's bits
void memory_unload_segment(Segment *segment);

///  @brief deletes a page of the memory
///  @param id of the owner process
///  @details finds the first used page of the process and frees it
void memory_delete_page(int id);

///  @brief swaps a segment
///  @details uses the second chance algorithm to choose the segment being
///  swapped
void swap_segment();

#endif