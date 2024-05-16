#include "cpu.h"

/// Necessary for global variables
extern Scheduler *scheduler;
extern sem_t process_semaphore;
extern Memory *memory;
extern List *PCB;

void init_cpu() {
  pthread_t cpu_id;

  pthread_attr_t cpu_attr;

  pthread_attr_init(&cpu_attr);

  pthread_attr_setscope(&cpu_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&cpu_id, NULL, (void *)cpu, NULL);
}

void cpu() {
  while (1) {  
    while(!scheduler->running_process);                        /// constantly running
    //if (!scheduler->running_process) { /// no running process
      //forward_scheduling();            /// schedules another process
    //} else {                           /// there is a scheduled process already
      while (scheduler->running_process &&
             scheduler->running_process->PC <
                 scheduler->running_process->segment->num_instructions &&
             scheduler->running_process->remaining_time >
                 0) { /// while the process is still running

        sem_wait(&process_semaphore);

        Process *running = scheduler->running_process;

        if (!running->segment
                 ->present_bit) { // segment is not loaded in the memory
          memory_load_syscall();
        } else {
          running->segment->used_bit = 1;

          process_instruction(
              running,
              running->segment
                  ->instructions[running->PC]); // aumenta PC aqui se der certo
        }

        sem_post(&process_semaphore);
      }

      /// exited while, that means the process stopped running, either because
      /// it was interrupted, because it has finished running, or because it
      /// used all of its quantum time

      if (scheduler->running_process->PC >=
          scheduler->running_process->segment->num_instructions) { /// finished

        process_finish_syscall(); // process finished
      }

      if (scheduler->running_process->remaining_time <=
          0) { // completed the quantum time
        // quantum tima acabopu = continua ready mas reescalona
        process_interrupt(QUANTUM_TIME_INTERRUPTION);
      }
    }
  //}
}

void process_instruction(Process *process, Instruction instruction) {
  switch (instruction.opcode) {
  /// EXEC has the format EXEC X, where X is the necessary time to execute  a
  /// given instruction
  case EXEC:
    process->remaining_time -= instruction.operand;
    /// updates program counter
    process->PC++;
    break;

    /// later
  case READ:
    break;

  /// later
  case WRITE:
    break;

  case P:
    char semaphore_p_id = instruction.semaphore;
    Node *semaphore_p_node = find(memory->semaphores, &semaphore_p_id);

    Semaphore *semaphore_p = (Semaphore *)semaphore_p_node;
    FLAGS result = semaphore_p_syscall(semaphore_p);

    if (result == SUCCESS) {
      scheduler->running_process->remaining_time -=
          200; /// 200 time required to semaphore
      scheduler->running_process->PC++;
    } else {
      process_interrupt(SEMAPHORE_INTERRUPTION);
    }

    break;

  case V:
    char semaphore_v_id = instruction.semaphore;
    Node *semaphore_v_node = find(memory->semaphores, &semaphore_v_id);

    Semaphore *semaphore_v = (Semaphore *)semaphore_v_node;
    semaphore_v_syscall(semaphore_v);
    scheduler->running_process->PC++;

    break;

  /// later
  case PRINT:
    break;

  default:
    printf("Error: the opcode is invalid");
    exit(0);
  }
}

void process_interrupt(INTERRUPTION_TYPE TYPE) {

  if(scheduler->running_process){
    if (TYPE == SEMAPHORE_INTERRUPTION)
      scheduler->running_process->status = WAITING;
    else
      scheduler->running_process->status = READY;
  }
  /// re-schedules
  forward_scheduling();
}

FLAGS semaphore_p_syscall(Semaphore *semaphore) {
  /// if there are no elements in the semaphore's queue
  if (semaphore->owner_id == -1) {
    semaphore->owner_id = scheduler->running_process->id;
    return SUCCESS;
  }
  /// enqueue the process if there are elements in the queue

  push(semaphore->processes_waiting, scheduler->running_process);
  return FAILURE;
}

void semaphore_v_syscall(Semaphore *semaphore) {
  semaphore->owner_id = -1;

  Process *new_process = (Process *)pop(semaphore->processes_waiting);

  if (new_process) {
    new_process->status = READY;

    add_process_scheduler(new_process);

    semaphore->owner_id = new_process->id;
  }
}

void process_finish_syscall() {
  /// delete the scheduler from the PCB
  delete_list(PCB, scheduler->running_process);

  /// calls the scheduler to advance scheduling
  forward_scheduling();
}

void memory_load_syscall() {
  scheduler->running_process->status = WAITING;

  memory_load_requisition();
  /// change process status and calls forward_scheduling to remove it
  /// and schedule the next running process
  /// OBS process interrupt qur mantem ready e reescalona
  process_interrupt(MEMORY_INTERRPUTION);
}

void process_create_syscall(char *filename) {
  Process *new_process = create_process(filename);

  push(PCB, new_process); /// adding the new process to the OS's PCB list

  add_process_scheduler(new_process);
  // interrompar quem está rodando (mantendo ready) e reescalonando
  process_interrupt(NEW_PROCESS_INTERRUPTION);

  printf("saiu do interrupr");
}

void memory_load_requisition() {
  Process *process = scheduler->running_process;

  if (memory->num_free_pages >=
      process->segment->num_pages) { /// there is space available in the memory
                                     /// for the pages
    // creating the pages and inserting them into memory's page table
    for (int i = 0; i < process->segment->num_pages; i++) {
      Page *new_page = malloc(sizeof(Page));
      new_page->process_id = process->id;
      new_page->segment_id = process->segment->id;

      add_page_memory(new_page);
    }
  } else {
    /// second chance
  }
}

void add_page_memory(
    Page *new_page) { // percorrer páginas e adicionar se achar livre no meio.
                      // Se não achar, adicionar depois
  int i = 0;

  while (memory->pages[i].free == 0 &&
         i < NUM_PAGES) { /// searches first free page
    i++;
  }

  memory->pages[i].number = new_page->number;
  memory->pages[i].process_id = new_page->process_id;
  memory->pages[i].segment_id = new_page->segment_id;
  memory->pages[i].free = 0;

  memory->num_free_pages--;
}
