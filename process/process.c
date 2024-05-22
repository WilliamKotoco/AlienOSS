#include "process.h"

extern int processes_id;

/// Necessary for using global variables defined in main.c
extern Memory *memory;
extern List *PCB;

int compare_processes(void *d1, void *d2) {
  Process *p1 = (Process *)d1;

  int d11 = p1->id;

  int *d22 = (int *)d2;
  int d23 = *d22;

  if (d11 == d23) {
    return 1;
  } else {
    return 0;
  }
}

Process *create_process(char *program_name) {
  /// alocate memory for the new process
  Process *new_process = malloc(sizeof(Process));

  new_process->id = processes_id++;
  new_process->PC = 0;

  /// reads program header and stores in new_process
  long final_header = read_program_header(program_name, new_process);

  /// error occured during the reading of the header or the instructions
  if ((final_header == -1) ||
      (read_program_instructions(program_name, new_process, final_header) ==
       -1)) {
    exit(1);
  }

  new_process->status = READY;

  return new_process;
}

long read_program_header(char *program_name, Process *process) {
  /// opens given file
  FILE *fp = fopen(program_name, "r");

  if (!fp) { /// cannot open the file
    printf("Error opening the program named %s", program_name);
    return -1;
  }

  char buffer[100];
  int seg_id, seg_size;

  /// reading first line, the name of the program
  fgets(buffer, 99, fp);
  buffer[strlen(buffer) - 1] = '\0';
  process->name = strdup(buffer);

  /// reading the process segment id
  fgets(buffer, 99, fp);
  buffer[strlen(buffer) - 1] = '\0';
  seg_id = strtol(buffer, NULL, 10); /// converting string to integer

  /// process priority
  fgets(buffer, 99, fp);
  buffer[strlen(buffer) - 1] = '\0';
  process->priority = strtol(buffer, NULL, 10);

  /// process segment size
  fgets(buffer, 99, fp);
  buffer[strlen(buffer) - 1] = '\0';
  seg_size = strtol(buffer, NULL, 10);

  char ch;
  while ((ch = getc(fp)) != '\n') { /// semaphore line
    if (ch == ' ') {
      continue;
    }

    /// ch is a semaphore name
    Node *node_found = find(memory->semaphores,
                            &ch); /// check if the semaphore is already created

    if (!node_found) { // this program is the first one to use this semaphore
      Semaphore *new_semaphore =
          malloc(sizeof(Semaphore)); /// create a new semaphore

      new_semaphore->name = ch;
      new_semaphore->owner_id = -1; /// no owner yet
      new_semaphore->processes_waiting =
          create_list(sizeof(Process), compare_processes);

      push(memory->semaphores, new_semaphore); /// adds semaphore to OS's list
    }
  }

  // create the process's segment
  Segment *process_segment = malloc(sizeof(Segment));

  process_segment->id = seg_id;
  process_segment->present_bit = 0; /// segment is in the disk at first
  process_segment->used_bit = 0;    /// it has also not been used
  process_segment->size =
      seg_size * KBYTE; /// converting the segment size from KB to bytes
  process_segment->num_pages = ceil(
      process_segment->size / PAGE_SIZE); /// calculating the number of pages
                                          /// needed to store the program data

  process->segment = process_segment;

  return ftell(fp); /// position on the file of the end of the program header
}

int read_program_instructions(char *program_name, Process *process,
                              long final_header) {
  FILE *fp = fopen(program_name, "r");

  if (!fp) { /// cannot open the file
    printf("Error opening the program named %s", program_name);
    return -1;
  }

  char buffer[100];

  fseek(fp, final_header, SEEK_SET); /// end of header

  fgets(buffer, 99,
        fp); // begin of instructions block (next line from the header)
  long instr_begin = ftell(fp);

  int num_instr = 0;

  /// scrolling through the file counting the number of instructions
  while (fgets(buffer, 99, fp)) { /// one instrcution = one line on the file
    num_instr++;
  }

  fseek(fp, instr_begin, SEEK_SET); /// repositioning the file pointer to the
                                    /// begin of the instructions block

  process->segment->num_instructions = num_instr;
  process->segment->instructions = (Instruction *)malloc(
      sizeof(Instruction) * num_instr); /// allocating the instructions array

  int i = 0;
  while (fgets(buffer, 99, fp)) { /// for every instruction
    if (buffer[0] == 'P' ||
        buffer[0] == 'V') { /// instruction related to a semaphore
      if (buffer[0] == 'P') {
        process->segment->instructions[i].opcode = P;
      } else {
        process->segment->instructions[i].opcode = V;
      }

      process->segment->instructions[i].semaphore =
          buffer[2]; /// semaphore name
    } else {
      /// instruction = OpCode [blankspace] operator
      char *left = strtok(buffer, " "); // left part of the instruction
      char *right = strtok(NULL, " ");  // right part of the instruction

      /// verifying the OpCode, attribuing the related enum value
      if (strcmp(left, "exec") == 0)
        process->segment->instructions[i].opcode = EXEC;
      else if (strcmp(left, "read") == 0)
        process->segment->instructions[i].opcode = READ;
      else if (strcmp(left, "write") == 0)
        process->segment->instructions[i].opcode = WRITE;
      else if (strcmp(left, "print") == 0)
        process->segment->instructions[i].opcode = PRINT;

      /// instructions operand
      process->segment->instructions[i].operand =
          strtol(right, NULL, 10); /// converting string to integer
    }

    i++;
  }

  return 1;
}
