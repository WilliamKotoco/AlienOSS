#include "instruction.h"

const char *get_opcode_string(Opcode opcode) {
  const char *opcode_string[] = {"EXEC", "READ", "WRITE", "P", "V", "PRINT"};

  return opcode_string[opcode];
}
