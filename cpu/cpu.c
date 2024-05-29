#include "cpu.h"
#include <semaphore.h>
/// Necessary for global variables
extern Scheduler *scheduler;
extern sem_t process_semaphore;
extern Memory *memory;
extern List *PCB;
extern bool new_process;

void init_cpu() {
  pthread_t cpu_id;

  pthread_attr_t cpu_attr;

  pthread_attr_init(&cpu_attr);

  pthread_attr_setscope(&cpu_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&cpu_id, NULL, (void *)cpu, NULL);
}

void cpu() {
  while (1) {
    /// waits until there is a scheduled or a new process
    while (!scheduler->running_process && new_process == false)
      ;

    if (new_process) { /// new process created
      process_interrupt(NEW_PROCESS_INTERRUPTION);
      update_new_process_flag(false);
    }

    //// there is a scheduled process
    /// it will continue running until it has used its quantum time, it has
    /// finished, or it has been forcebly taken out of the CPU
    while (scheduler->running_process &&
           scheduler->running_process->PC <
               scheduler->running_process->segment->num_instructions &&
           scheduler->running_process->remaining_time > 0 &&
           new_process == false) {

      Process *running = scheduler->running_process;

      if (!running->segment->present_bit) { /// data is not loaded
        memory_load_syscall(running);
      } else {
        running->segment->used_bit = 1; /// segment's data has been used

        /// process current instruction is processed
        process_instruction(running,
                            running->segment->instructions[running->PC]);
      }
    }

    /// exited while, that means the process stopped running, either because
    /// it was interrupted, because it has finished, or because it used all
    /// of its quantum time

    /// all inscructions were executed
    if (scheduler->running_process->PC >=
        scheduler->running_process->segment->num_instructions) {
      process_finish_syscall(scheduler->running_process); /// process finished
    }

    /// completed the quantum time
    else if (scheduler->running_process->remaining_time <= 0) {
      process_interrupt(QUANTUM_TIME_INTERRUPTION);
    }
  }
}

void process_instruction(Process *process, Instruction instruction) {
  FLAGS flag = SUCCESS;

  switch (instruction.opcode) {
  case EXEC:
    /// EXEC has the format EXEC X, where X is the necessary time to execute
    process->remaining_time -= instruction.operand;
    /// updates program counter
    process->PC++;

    print_execution(EXEC, process, instruction, flag);

    break;

  case READ:
    process->PC++;
    break;

  case WRITE:
    process->PC++;
    break;

  case P:
    /// P(s), where s is the semaphore the process is waiting for
    char semaphore_p_id = instruction.semaphore;

    /// searches for the semaphore in the semaphores list
    Node *semaphore_p_node = find(memory->semaphores, &semaphore_p_id);
    Semaphore *semaphore_p = (Semaphore *)semaphore_p_node->data;

    flag = semaphore_p_syscall(process, semaphore_p);

    process->PC++;

    print_execution(EXEC, process, instruction, flag);

    if (flag == SUCCESS) {
      scheduler->running_process->remaining_time -=
          200; /// time required to acquire
    } else {
      process_interrupt(SEMAPHORE_INTERRUPTION);
    }
    break;

  case V:
    /// V(s), where s is the semaphore the process is posting
    char semaphore_v_id = instruction.semaphore;

    /// searches for the semaphore in the semaphores list
    Node *semaphore_v_node = find(memory->semaphores, &semaphore_v_id);
    Semaphore *semaphore_v = (Semaphore *)semaphore_v_node->data;

    process->PC++;

    print_execution(EXEC, process, instruction, flag);

    /// frees semaphore
    semaphore_v_syscall(semaphore_v);

    break;

  case PRINT:
    process->PC++;
    break;

  default:
    printf("Error: the opcode is invalid");

    exit(0);
  }
}

void process_interrupt(INTERRUPTION_TYPE TYPE) {
  sleep(1);

  if (scheduler->running_process) {
    if (TYPE == SEMAPHORE_INTERRUPTION)
      scheduler->running_process->status = WAITING;
    else
      scheduler->running_process->status = READY;
  }

  if (scheduler->running_process) {
    print_interruption(TYPE, scheduler->running_process);
  }

  /// re-schedules
  forward_scheduling();

  if (new_process)
    update_new_process_flag(false);
}
FLAGS semaphore_p_syscall(Process *process, Semaphore *semaphore) {

  /// if there isn't a element with the semaphore
  if (semaphore->owner_id == -1 || semaphore->owner_id == process->id) {
    semaphore->owner_id = scheduler->running_process->id;

    return SUCCESS;
  }

  /// enqueue the process if there is already a process
  push(semaphore->processes_waiting, scheduler->running_process);

  return FAILURE;
}

void semaphore_v_syscall(Semaphore *semaphore) {
  /// frees semaphore
  semaphore->owner_id = -1;

  /// checks if there is a process waiting for this semaphore
  Node *new_process = pop(semaphore->processes_waiting);

  if (new_process) {
    /// the process has acquired the semaphore and is ready to run
    Process *new_process_data = (Process *)new_process->data;

    new_process_data->status = READY;
    add_process_scheduler(new_process_data);

    print_syscall(V_SYSCALL, new_process_data, semaphore->name);

    semaphore->owner_id = new_process_data->id;
  }
}

void process_finish_syscall(Process *process) {
  /// changes status and deletes from the PCB
  process->status = FINISHED;
  delete_list(PCB, &process->id);

  /// unloads segment from the memory
  memory_unload_segment(process->segment);

  print_syscall(FINISH_SYSCALL, process, ' ');

  /// calls the scheduler to advance scheduling
  forward_scheduling();
}

void memory_load_syscall(Process *process) {
  process->status = WAITING;

  print_syscall(MEMORY_LOAD_SYSCALL, process, ' ');

  /// loads segment
  memory_load_requisition(process);

  process->segment->present_bit = 1;
  process->segment->dirty_bit = 0;

  print_syscall(MEMORY_FINISH_SYSCALL, process, ' ');

  /// change process status and calls forward_scheduling to remove it
  /// and schedule the next running process
  process_interrupt(MEMORY_INTERRPUTION);
}

void process_create_syscall(char *filename) {
  Process *new_process = create_process(filename);

  /// adds the new process to the OS's PCB list and scheduler's list
  push(PCB, new_process);
  add_process_scheduler(new_process);

  print_syscall(CREATE_PROCESS_SYSCALL, new_process, ' ');

  /// interrupts running process
  update_new_process_flag(true);
}

void memory_load_requisition(Process *process) {
  /// not enough space, swapping needed
  while (memory->num_free_pages < process->segment->num_pages) {
    swap_segment();
  }

  /// there is enough space available in the memory
  load_segment(process);

  /// inserts segment in the segments list
  push(memory->segments, process->segment);
}

void load_segment(Process *process) {
  /// creates the pages and inserts them into memory's page table
  for (int i = 0; i < process->segment->num_pages; i++) {
    Page *new_page = malloc(sizeof(Page));

    new_page->process_id = process->id;
    new_page->segment_id = process->segment->id;

    add_page_memory(new_page);
  }
}

void add_page_memory(Page *new_page) {
  int i = 0;

  /// searches first free page
  while (memory->pages[i].free == 1 && i < NUM_PAGES) {
    i++;
  }

  /// loads page into memory
  memory->pages[i].number = i;
  memory->pages[i].process_id = new_page->process_id;
  memory->pages[i].segment_id = new_page->segment_id;
  memory->pages[i].free = 0;

  memory->num_free_pages--;
}

void memory_unload_segment(Segment *segment) {
  for (int i = 0; i < segment->num_pages; i++) {
    memory_delete_page(segment->id);
  }

  segment->dirty_bit = 0;
  segment->present_bit = 0;
  segment->used_bit = 0;

  /// remove segment from the list, as it is not loaded anymore
  delete_list(memory->segments, &segment->id);
}

void memory_delete_page(int id) {
  int i = 0;

  /// searches for the first used page of the process
  while (i < NUM_PAGES) {
    if (memory->pages[i].free == 0 && memory->pages[i].segment_id == id) {
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

void swap_segment() {
  Node *tmp = memory->next_swapped == NULL ? memory->segments->header
                                           : memory->next_swapped;

  /// circular search in the list
  while (tmp) {
    Segment *tmp_seg = (Segment *)tmp->data;

    /// if the segment has not been used, it is unloaded
    if (tmp_seg->used_bit == 0) {
      memory_unload_segment(tmp_seg);

      // sleep(1);
      // char message[256];
      // snprintf(message, sizeof(message), "Swapping tirou %d", tmp_seg->id);
      // append_log_message(message, MEMORY_LOG);

      return;
    } else {
      /// gives a used segment a second chance
      tmp_seg->used_bit = 0;
    }

    /// last node
    if (!tmp->next) {
      /// makes the search ciruclar
      tmp = memory->segments->header;
    } else {
      tmp = tmp->next;
    }
  }

  /// uploades the next segment to be unloaded
  memory->next_swapped = tmp->next;
}

void update_new_process_flag(bool val) {
  sem_wait(&process_semaphore);
  new_process = val;
  sem_post(&process_semaphore);
}