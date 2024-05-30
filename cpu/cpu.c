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

/// @brief update global variable using a mutex
/// @param val  true or false
static void update_new_process_flag(bool val) {
  sem_wait(&process_semaphore);
  new_process = val;
  sem_post(&process_semaphore);
}

///  @brief  Emulates the execution of a given instruction
///  @param process process being executed
///  @param instruction instruction to be executed
///
///  @details identifies the given instruction and calls the specific syscall

static void process_instruction(Process *process, Instruction *instruction) {
  FLAGS flag = SUCCESS;

  switch (instruction->opcode) {
  case EXEC:
    /// EXEC has the format EXEC X, where X is the necessary time to execute

    /// for each call, it executes one unit of the exec block
    process->segment->exec++;

    process->remaining_time--;
    instruction->operand--;

    /// execution of exec block is over
    if (instruction->operand == 0) {
      /// updates program counter
      process->PC++;

      instruction->operand = process->segment->exec;

      print_execution(process, instruction, flag);
    }

    break;

  case READ:
    process->PC++;
    break;

  case WRITE:
    process->PC++;
    break;

  case P:
    /// P(s), where s is the semaphore the process is waiting for
    char semaphore_p_id = instruction->semaphore;

    /// searches for the semaphore in the semaphores list
    Node *semaphore_p_node = find(memory->semaphores, &semaphore_p_id);
    Semaphore *semaphore_p = (Semaphore *)semaphore_p_node->data;

    flag = semaphore_p_syscall(process, semaphore_p);

    process->PC++;

    print_execution(process, instruction, flag);

    if (flag == SUCCESS) {
      scheduler->running_process->remaining_time -=
          200; /// time required to acquire
    } else {
      process_interrupt(SEMAPHORE_INTERRUPTION);
    }
    break;

  case V:
    /// V(s), where s is the semaphore the process is posting
    char semaphore_v_id = instruction->semaphore;

    /// searches for the semaphore in the semaphores list
    Node *semaphore_v_node = find(memory->semaphores, &semaphore_v_id);
    Semaphore *semaphore_v = (Semaphore *)semaphore_v_node->data;

    process->PC++;

    print_execution(process, instruction, flag);

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
                            &(running->segment->instructions[running->PC]));
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

void process_interrupt(INTERRUPTION_TYPE TYPE) {
  sleep(3);

  /// if there is a running process, interrupt it
  if (scheduler->running_process) {
    if (TYPE == SEMAPHORE_INTERRUPTION)
      scheduler->running_process->status = WAITING;
    else
      scheduler->running_process->status = READY;

    print_interruption(TYPE, scheduler->running_process);
  }

  /// re-schedules
  forward_scheduling();

  if (new_process)
    update_new_process_flag(false);
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

    print_syscall(SEMAPHORE_SYSCALL, new_process_data, semaphore->name);

    semaphore->owner_id = new_process_data->id;
  }
}

void process_finish_syscall(Process *process) {
  /// changes status and deletes from the PCB
  process->status = FINISHED;
  delete_list(PCB, &process->id);

  /// unloads segment from the memory
  memory_unload_segment(process->segment);
  free(process->segment);

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

  /// change process status and calls forward_scheduling to remove it
  /// and schedule the next running process
  process_interrupt(MEMORY_INTERRPUTION);

  print_syscall(MEMORY_FINISH_SYSCALL, process, ' ');
}
