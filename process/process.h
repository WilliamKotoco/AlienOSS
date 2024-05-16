#include "../memory/memory.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @enum states a process can be in
typedef enum status { READY, WAITING, FINISHED, RUNNING } Status;

/// info needed from a process, obtained from the synthetic program
typedef struct process {
  char *name; //!< program name
  int id;     //!< process identifier

  int priority;  ///< process priority, smaller the number, higher the priority
  Status status; //!< @enum process status
  Segment *segment; ///< @struct memory segment, groups the program data
  int PC; ///< program counter, stores the offset of the next instruction to be
          ///< executed
  long remaining_time;
} Process;

/// @brief Compares two processes based on their id. Used on the generic list.
/// @param d1 a process
/// @param d2 the process being compared to d1
/// @return 1 if d1 and d2 are the same process, and 0 otherwise
int compare_processes(void *d1, void *d2);

/// Creates a new process, reading its information and instructions from the
/// program file, and adding the new process to the PCB, OS's list of processes
/// @param program_name name of the program file, submitted by the user
Process *create_process(char *program_name);

/// Auxiliary funtion to create a new process. Reads the header of the program
/// from the file and stores it in the process.
/// @param program_name name of the program file, submitted by the user
/// @param process the process to be created
/// @return -1 if the file cannot be opened, otherwise the position of the end
/// of the header in the file
long read_program_header(char *program_name, Process *process);

/// Auxiliary funtion to create a new process. Reads the instructions of the
/// program from the file and stores it in the process segment.
/// @param program_name name of the program file, submitted by the user
/// @param process the process to be created
/// @param final_header the position of the end of the header in the file (the
/// begin of the instructions block)
/// @return -1 if the file cannot be opened, otherwise 1
int read_program_instructions(char *program_name, Process *process,
                              long final_header);
