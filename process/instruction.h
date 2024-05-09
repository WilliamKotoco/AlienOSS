#include <stdio.h>
#include <stdlib.h>

/// @enum instruction's OpCodes
typedef enum opcode { EXEC, READ, WRITE, P, V, PRINT } Opcode;

///
typedef struct instruction {
  Opcode opcode;
  int operand; /*OBS VER SE DÁ PRO SEMÁFORO*/
} Instruction;