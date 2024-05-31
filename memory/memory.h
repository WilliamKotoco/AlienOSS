#ifndef MEMORY_H
#define MEMORY_H

typedef struct process Process;

#include "../process/instruction.h"
#include "../process/process.h"
#include "../semaphore/semaphore.h"
#include <stdio.h>
#include <stdlib.h>

//#define MEMORY_SIZE 1073741824
#define MEMORY_SIZE 73728
#define PAGE_SIZE 8192
//#define NUM_PAGES 131072
#define NUM_PAGES 9

#define KBYTE 1024

/// Represents a memory page, identified by its number and that references the
/// segment and process it belongs to. A tuple on the memory's page table.
typedef struct page {
  unsigned number;     //!< page number, identifier
  unsigned segment_id; //!< reference to the segment this page belongs to
  unsigned process_id; //!< reference to the process that owns the segment/page
  unsigned free : 1;   //!< indicates whether the page is free
} Page;

/// Represents a process segment, with its size, pages and instructions.
typedef struct segment {
  unsigned id;        //!< segment identifier
  unsigned size;      //!< size of the program data
  unsigned num_pages; //!< number of memory pages the segment occupies, its size
                      //!< / PAGE_SIZE
  unsigned num_instructions; //!< number of instructions the program possesses
  long exec;                 //!< execution timestamp or other long data
  Instruction *instructions; //!< array of the process instructions
  unsigned
      present_bit : 1; //!< flag that indicates whether the segment is in memory
  unsigned dirty_bit : 1; //!< indicates whether the data has been modified
                          //!< since it came to memory
  unsigned used_bit : 1;  //!< indicates if the data has been read or written
} Segment;

/// Represents the memory of the system, contains the page table, needed for the
/// OS management.
typedef struct memory {
  Page *pages; ///< memory table, array of its pages
  unsigned
      num_free_pages; //!< number of free pages, NUM_PAGES - List *pages length
  List *segments;     //!< segments used for swapping
  Node *next_swapped; //!< pointer to the next swapped node
  List *semaphores;   //!< semaphores managed by the OS
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

///  @brief Unloads a process' segment
///  @param segment segment to be unloaded
///  @details removes each of the segment's pages and updates the segment's bits
void memory_unload_segment(Segment *segment);

#endif