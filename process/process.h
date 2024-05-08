#include "../memory/memory.h"
#include "instruction.h"
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