#ifndef PRINT_H
#define PRINT_H

#include "../process/process.h"
#include "../list/list.h"

typedef struct printRequest {
  unsigned id;

  Process *process;

  int time;
} PrintRequest;

/// @brief Initializes the print list and thread
void init_print();

/// @brief Creates a print request
/// @param process the process that made the request
/// @param instruction the instruction
/// @details Based on the process and the operand of the instruction, creates a print requisition and adds it to the list
void create_print_request(Process *process, Instruction *instruction);

/// @brief Emulates the execution of print requisitions
/// @details Whenever there is a print requisition on the list, attends it and interrupts the current process
void print();

#endif