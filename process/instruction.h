#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdio.h>
#include <stdlib.h>

/// @enum instruction's OpCodes
typedef enum opcode { EXEC, READ, WRITE, P, V, PRINT } Opcode;

/// Represents one intruction of a program. Made up of a OpCode and its operand.
typedef struct instruction {
  Opcode opcode; //!< instruction OpCode
  int operand;
  char semaphore;
} Instruction;

#endif