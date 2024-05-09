#include "process.h"

int compare_processes(void *d1, void *d2) {
  Process *p1 = (Process *)d1;

  int d11 = p1->id;

  int *d22 = (int *)d2;

  if (d11 == *d22) {
    return 1;
  } else {
    return 0;
  }
}

void create_process(char *program_name){
  Process *new_process = malloc(sizeof(Process));
  new_process->id = processes_id++;
  new_process->PC = 0;

  if((read_program_header(program_name, new_process) == -1) || (read_program_instructions(program_name, new_process) == -1) ){
    return;
  }

  new_process->status = READY;

  push(PCB, new_process);
}

int read_program_header(char *program_name, Process *process){
  FILE *fp = fopen(program_name, "r");

  if(!fp){
    printf("Error opening the program named %s", program_name);
    return -1;
  }

  int seg_id, seg_size;

  fscanf(fp, "%s", process->name);
  fscanf(fp, "%d", &seg_id);
  fscanf(fp, "%d", &process->priority);
  fscanf(fp, "%d", &seg_size);

  char ch;
  while((ch = getc (fp)) != '\n'){ // semaphore line
      if(ch == ' '){
         continue;
      }
      
      // ch is a semaphore name
      Node *node_found = find(memory->semaphores, ch);

      if(!node_found){ // this program is the first one to use this semaphore
        Semaphore *new_semaphore = malloc(sizeof(Semaphore));
        new_semaphore->name = ch;
        new_semaphore->owner_id = -1;
        new_semaphore->processes_waiting = create_list(sizeof(Process), compare_processes);
        push(memory->semaphores, new_semaphore);
      }
   }

  // criar segmento do processo
  Segment *process_segment = malloc(sizeof(Segment));
  process_segment->id = seg_id;
  process_segment->in_memory = 0;
  process_segment->size = seg_size * KBYTE;
  process_segment->num_pages = ceil(process_segment->size / PAGE_SIZE);

  process->segment = process_segment;

  return 1;
}

int read_program_instructions(char *program_name, Process *process){
  FILE *fp = fopen(program_name, "r");

  if(!fp){
    printf("Error opening the program named %s", program_name);
    return -1;
  }

  char buffer[100];

  fgets(buffer, 99, fp); // begin of instructions block
  long instr_begin = ftell(fp);

  int num_instr = 0;

  while(fgets(buffer, 99, fp)){ // one instrcution = one line on the file
    num_instr++;
  }

  fseek(fp, instr_begin, SEEK_SET); // repositioning the file pointer to the begin of the instructions block
  
  process->segment->num_instructions = num_instr;
  process->segment->instructions = (Instruction *) malloc(sizeof(Instruction) * num_instr); // allocate instructions array

  int i = 0;
  while (fgets(buffer, 99, fp)){ // for every instruction
    if (buffer[0] == 'P' || buffer[0] == 'V'){ // instruction related to a semaphore
      if(buffer[0] == 'P'){
        process->segment->instructions[i].opcode = P; 
      } else{
        process->segment->instructions[i].opcode = V;
      }

      process->segment->instructions[i].semaphore = buffer[2];
    } else {
      char* left = strtok(buffer, " "); // left part of the instruction, the OpCode
      char *right = strtok(NULL, " "); // right part of the instruction, the operand

      if (strcmp(left, "exec") == 0)
        process->segment->instructions[i].opcode = EXEC;
      else if (strcmp(left, "read") == 0)
        process->segment->instructions[i].opcode = READ;
      else if (strcmp(left, "write") == 0)
        process->segment->instructions[i].opcode = WRITE;
      else if (strcmp(left, "print") == 0)
        process->segment->instructions[i].opcode = PRINT;

      process->segment->instructions[i].operand = atoi(right);
    }

    i++;
  }

  return 1;
}