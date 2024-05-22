#include "../process/instruction.h"
#include "../semaphore/semaphore.h"
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 1073742000
#define PAGE_SIZE 8192
//#define NUM_PAGES 131072
#define NUM_PAGES 5

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

/// Represents the memory of the system, contains the page table, needed for the
/// OS management.
typedef struct memory {
  Page *pages;        ///< memory table, array of its pages
  int num_free_pages; //!< number of free pages, NUM_PAGES - List *pages length

  List *segments;     //!< segments used for swapping

  List *semaphores;   //!< semaphores managed by the OS
} Memory;

/// Represents a process segment, with its size, pages and instructions.
typedef struct segment {
  int id;          //!< segment identifier
  int size;        //!< size of the program data
  int num_pages;   ///< number of memory pages the segment occupies, its size /
                   ///< PAGE_SIZE
  int present_bit; //!< flag that indicates whether the segment is in memory
  Instruction *instructions; //!< array of the process instructions
  int num_instructions;      //!< number of instructions the program posesses

  int dirty_bit; //!< indicates whether the data has been modified since it came
                 //!< to memory
  int used_bit;  //!< indicates if the data has been read or written
} Segment;

/// @brief Compares two pages based on their number. Used on the generic list.
/// @param d1 a page
/// @param d2 the id of page being compared to d1
/// @return 1 if d1 and d2 are the same page, and 0 otherwise
int compare_pages(void *d1, void *d2);

/// @brief Initializes the system's memory
/// @return the memory structure initialized
Memory *init_memory();