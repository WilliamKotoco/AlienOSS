#include "../memory/memory.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



/// @enum states a process can be in
typedef enum status { READY, WAITING, FINISHED } Status;

/// info needed from a process, obtained from the synthetic program
typedef struct process {
  char *name;    //!< program name
  int id;        //!< process identifier
  int priority;  ///< process priority, smaller the number, higher the priority
  Status status; //!< @enum process status
  Segment *segment; ///< @struct memory segment, groups the program data
  int PC; ///< program counter, stores the offset of the next instruction to be
          ///< executed
} Process;


/// @brief Compares two processes based on their id. Used on the generic list.
/// @param d1 a process
/// @param d2 the process being compared to d1
/// @return 1 if d1 and d2 are the same process, and 0 otherwise
int compare_processes(void *d1, void *d2);

void create_process(char *program_name);