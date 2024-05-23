#include "cpu.h"
#include <semaphore.h>

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
    while (!scheduler->running_process) /// no scheduled process
      ;

    //// there is a scheduled process
    /// it will continue running until it has used its quantum time, it has
    /// finished, or it has been forcebly taken out of the CPU
    while (scheduler->running_process &&
           scheduler->running_process->PC <
               scheduler->running_process->segment->num_instructions &&
           scheduler->running_process->remaining_time > 0) {
      sem_wait(&process_semaphore); /// ensures that the running process does
                                    /// not change in the middle of executing a
                                    /// instruction

      Process *running = scheduler->running_process;

      if (!running->segment->present_bit) { /// data is not loaded in the memory
        memory_load_syscall(running);
      } else {
        running->segment->used_bit = 1; /// segment's data has been used

        if (process_instruction(running,
                                running->segment->instructions[running->PC]) ==
            SUCCESS) {
          sem_post(&process_semaphore); /// the instruction was executed, can
                                        /// now change the running process
                                        /// safely (if desired)
        }
      }
    }

    /// exited while, that means the process stopped running, either because
    /// it was interrupted, because it has finished running, or because it
    /// used all of its quantum time

    if (scheduler->running_process->PC >=
        scheduler->running_process->segment->num_instructions) {
      /// all inscructions were executed
      process_finish_syscall(scheduler->running_process); /// process finished
    }

    else if (scheduler->running_process->remaining_time <= 0) {
      /// completed the quantum time
      process_interrupt(QUANTUM_TIME_INTERRUPTION);
    }
  }
}

FLAGS process_instruction(Process *process, Instruction instruction) {
  char message[256];
  sleep(1);

  switch (instruction.opcode) {
  case EXEC:
    /// EXEC has the format EXEC X, where X is the necessary time to execute
    process->remaining_time -= instruction.operand;

    /// updates program counter
    process->PC++;

    snprintf(message, sizeof(message), "Process %d executed for %d seconds",
             process->id, instruction.operand);
    append_log_message(message, PROCESS_LOG);

    return SUCCESS;

  case READ:
    process->PC++;

    return SUCCESS;

  case WRITE:
    process->PC++;

    return SUCCESS;

  case P:
    /// P(s), where s is the semaphore the process is waiting for
    char semaphore_p_id = instruction.semaphore;

    /// searches for the semaphore in the semaphores list
    Node *semaphore_p_node = find(memory->semaphores, &semaphore_p_id);
    Semaphore *semaphore_p = (Semaphore *)semaphore_p_node->data;

    FLAGS result = semaphore_p_syscall(process, semaphore_p);

    if (result == SUCCESS) {
      scheduler->running_process->remaining_time -=
          200; /// 200 time required to acquire
      scheduler->running_process->PC++;
      snprintf(
          message, sizeof(message),
          "Process %d acquired the semaphore %c and executed for 200 seconds",
          process->id, semaphore_p->name, instruction.operand);
      append_log_message(message, PROCESS_LOG);

      return SUCCESS;
    } else {
      snprintf(message, sizeof(message),
               "Process %d is waiting for the semaphore %c", process->id,
               semaphore_p->name);
      append_log_message(message, PROCESS_LOG);
      process_interrupt(SEMAPHORE_INTERRUPTION);

      return FAILURE;
    }

  case V:
    /// V(s), where s is the semaphore the process is posting
    char semaphore_v_id = instruction.semaphore;

    /// searches for the semaphore in the semaphores list
    Node *semaphore_v_node = find(memory->semaphores, &semaphore_v_id);
    Semaphore *semaphore_v = (Semaphore *)semaphore_v_node->data;

    /// frees semaphore
    semaphore_v_syscall(semaphore_v);

    scheduler->running_process->PC++;

    char message[256];
    snprintf(message, sizeof(message), "Process %d released the semaphore %c",
             process->id, semaphore_v->name);
    append_log_message(message, PROCESS_LOG);
    return SUCCESS;

  case PRINT:
    process->PC++;

    return SUCCESS;

  default:
    printf("Error: the opcode is invalid");
    exit(0);
  }
}

void process_interrupt(INTERRUPTION_TYPE TYPE) {
  if (scheduler->running_process) {
    /// any interruption created by the running process will make it stop
    /// running mid execution
    if (TYPE != NEW_PROCESS_INTERRUPTION) {
      /// makes it possible to reeschedule while the process has not finished
      /// executing the instruction
      sem_post(&process_semaphore);
    }

    if (TYPE == SEMAPHORE_INTERRUPTION)
      scheduler->running_process->status = WAITING;
    else
      scheduler->running_process->status = READY;
  }

  /// re-schedules
  forward_scheduling();
}

FLAGS semaphore_p_syscall(Process *process, Semaphore *semaphore) {
  /// if there isn't a element in the semaphore
  if (semaphore->owner_id == -1) {
    semaphore->owner_id = scheduler->running_process->id;

    return SUCCESS;
  }

  /// enqueue the process if there is already a process
  push(semaphore->processes_waiting, scheduler->running_process);

  return FAILURE;
}

void semaphore_v_syscall(Semaphore *semaphore) {
  semaphore->owner_id = -1;

  /// checks if there is a process waiting for this semaphore
  Process *new_process = (Process *)pop(semaphore->processes_waiting);

  if (new_process) {
    /// the process has acquired the semaphore and is ready to run
    new_process->status = READY;
    add_process_scheduler(new_process);

    semaphore->owner_id = new_process->id;
  }
}

void process_finish_syscall(Process *process) {
  /// changes status and deletes from the PCB
  scheduler->running_process->status = FINISHED;
  delete_list(PCB, &process->id);

  /// unloads segment from the memory
  memory_unload_segment(process);

  /// calls the scheduler to advance scheduling
  forward_scheduling();
}

void memory_load_syscall(Process *process) {
  process->status = WAITING;

  memory_load_requisition(process);

  process->segment->present_bit = 1;
  /// change process status and calls forward_scheduling to remove it
  /// and schedule the next running process
  /// OBS process interrupt qur mantem ready e reescalona
  process_interrupt(MEMORY_INTERRPUTION);
}

void process_create_syscall(char *filename) {
  Process *new_process = create_process(filename);

  /// adds the new process to the OS's PCB list and scheduler's list
  push(PCB, new_process);
  add_process_scheduler(new_process);

  char message[256];
  snprintf(message, sizeof(message), "Process %d created at ", new_process->id);

  append_log_message(message, PROCESS_LOG);

  /// interrupts running process
  process_interrupt(NEW_PROCESS_INTERRUPTION);
}

void memory_load_requisition(Process *process) {
  /// there is enough space available in the memory
  if (memory->num_free_pages >= process->segment->num_pages) {
    // creates the pages and inserts them into memory's page table
    for (int i = 0; i < process->segment->num_pages; i++) {
      Page *new_page = malloc(sizeof(Page));

      new_page->process_id = process->id;
      new_page->segment_id = process->segment->id;

      add_page_memory(new_page);
    }
  } else { /// not enough space, swapping
    /// second chance
  }
}

void add_page_memory(Page *new_page) {
  int i = 0;

  /// searches first free page
  while (memory->pages[i].free == 0 && i < NUM_PAGES) {
    i++;
  }

  /// loads page into memory
  memory->pages[i].number = new_page->number;
  memory->pages[i].process_id = new_page->process_id;
  memory->pages[i].segment_id = new_page->segment_id;
  memory->pages[i].free = 0;

  memory->num_free_pages--;
}

void memory_unload_segment(Process *process) {
  for (int i = 0; i < process->segment->num_pages; i++) {
    memory_delete_page(process->id);
  }

  process->segment->dirty_bit = 0;
  process->segment->present_bit = 0;
  process->segment->used_bit = 0;
}

void memory_delete_page(int id) {
  int i = 0;

  /// searches for the first used page of the process
  while (i < NUM_PAGES) {
    if (memory->pages[i].free == 0 && memory->pages[i].process_id == id) {
      break;
    }
    i++;
  }

  /// frees page
  memory->pages[i].free = 1;
  memory->pages[i].process_id = -1;
  memory->pages[i].segment_id = -1;

  memory->num_free_pages++;
}
