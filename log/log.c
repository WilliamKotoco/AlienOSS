#include "log.h"
#include <semaphore.h>
#include <string.h>

extern List *LOGS;
extern sem_t log_semaphore;

void append_log_message(char *message, LOG_TYPE log_type) {
  LogMessage *log_message = malloc(sizeof(LogMessage));

  strcpy(log_message->log_message, message);
  log_message->log_type = log_type;

  push(LOGS, log_message);

  /// wake up the thread responsible for printing new messages
  sem_post(&log_semaphore);
}

int compare_log(void *d1, void *d2) { return 1; }

void print_interruption(INTERRUPTION_TYPE type, Process *process_interrupted) {
  char message[256];

  /// considering the execution of a process can only be externally interrupted
  /// by a new process

  /// if a process was interrupted in the middle of a exec block
  Instruction instr_interrupted =
      process_interrupted->segment->instructions[process_interrupted->PC];

  sleep(1);

  switch (type) {
  case NEW_PROCESS_INTERRUPTION:

    if (instr_interrupted.opcode == EXEC) {
      instr_interrupted.operand = process_interrupted->segment->exec;

      print_execution(instr_interrupted.opcode, process_interrupted,
                      &instr_interrupted, SUCCESS);
    }

    snprintf(message, sizeof(message), "Process %d interrupted by new process",
             process_interrupted->id);

    break;

  case MEMORY_INTERRPUTION:
    snprintf(message, sizeof(message),
             "Process %d interrupted by memory request",
             process_interrupted->id);

    break;

  case QUANTUM_TIME_INTERRUPTION:
    /// if a process was interrupted in the middle of a exec block

    if (instr_interrupted.opcode == EXEC) {
      instr_interrupted.operand = process_interrupted->segment->exec;

      print_execution(instr_interrupted.opcode, process_interrupted,
                      &instr_interrupted, SUCCESS);
    }

    snprintf(message, sizeof(message), "Process %d interrupted by quantum time",
             process_interrupted->id);

    break;

  case SEMAPHORE_INTERRUPTION:
    snprintf(message, sizeof(message), "Process %d interrupted by semaphore",
             process_interrupted->id);

    break;
  }

  append_log_message(message, PROCESS_LOG);
}

void print_execution(Opcode opcode, Process *process, Instruction *instruction,
                     FLAGS flag) {
  char message[256];

  sleep(3);

  switch (instruction->opcode) {
  case EXEC:
    snprintf(message, sizeof(message), "Process %d executed for %d seconds",
             process->id, instruction->operand);

    process->segment->exec = 0;

    break;

  case P:
    if (flag == SUCCESS) {

      snprintf(message, sizeof(message), "Process %d acquired the semaphore %c",
               process->id, instruction->semaphore);
    } else {
      snprintf(message, sizeof(message),
               "Process %d is waiting for the semaphore %c", process->id,
               instruction->semaphore);
    }

    break;

  case V:
    snprintf(message, sizeof(message), "Process %d released the semaphore %c",
             process->id, instruction->semaphore);

    break;
  }

  append_log_message(message, PROCESS_LOG);
}

void print_ownership(int process_id, char semaphore_name) {
  char message[256];

  sleep(1);

  snprintf(message, sizeof(message),
           "Process %d is now the owner of semaphore %c", process_id,
           semaphore_name);

  append_log_message(message, PROCESS_LOG);
}

void print_finish(int process_id) {
  char message[256];

  sleep(1);

  snprintf(message, sizeof(message), "Process %d finished", process_id);

  append_log_message(message, PROCESS_LOG);
}

void print_syscall(SYSCALL syscall, Process *process, char semaphore_name) {
  char message[256];

  sleep(1);

  switch (syscall) {
  case V_SYSCALL:
    snprintf(message, sizeof(message),
             "Process %d is now the owner of semaphore %c", process->id,
             semaphore_name);

    break;

  case FINISH_SYSCALL:
    snprintf(message, sizeof(message), "Process %d finished", process->id);

    break;

  case MEMORY_LOAD_SYSCALL:
    snprintf(message, sizeof(message), "Memory load requisition for process %d",
             process->id);

    append_log_message(message, MEMORY_LOG);

    return;

  case CREATE_PROCESS_SYSCALL:
    snprintf(message, sizeof(message), "Process %d created", process->id);

    break;
  }

  append_log_message(message, PROCESS_LOG);
}

void print_scheduled(Process *process) {
  char message[256];

  sleep(1);

  snprintf(message, sizeof(message), "Process %d scheduled", process->id);

  append_log_message(message, PROCESS_LOG);
}
