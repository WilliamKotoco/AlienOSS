#include "log.h"
#include "../ui/main-window.h"
#include <semaphore.h>
#include <string.h>
extern List *LOGS;
extern sem_t log_semaphore;

void append_log_message(char *message, LOG_TYPE log_type, char *highlight) {
  LogMessage *log_message = malloc(sizeof(LogMessage));

  strcpy(log_message->log_message, message);
  log_message->log_type = log_type;

  push(LOGS, log_message);

  refresh_log(highlight);
}

int compare_log(void *d1, void *d2) { return 1; }

void print_interruption(INTERRUPTION_TYPE type, Process *process_interrupted) {
  char message[256];

  /// considering the execution of a process can only be externally interrupted
  /// by a new process

  Instruction instr_interrupted =
      process_interrupted->segment->instructions[process_interrupted->PC];

  sleep(1);

  /// for each interruption type, prints a message with the format
  /// process <name> was interrupted by <interruption type>
  switch (type) {
  case NEW_PROCESS_INTERRUPTION:

    /// if a process was interrupted in the middle of a exec block
    if (instr_interrupted.opcode == EXEC) {
      instr_interrupted.operand = process_interrupted->segment->exec;

      /// prints information about its execution (until being interrupted)
      print_execution(process_interrupted, &instr_interrupted, SUCCESS);
    }

    snprintf(message, sizeof(message), "Process %s interrupted by new process",
             process_interrupted->name);

    break;

  case MEMORY_INTERRPUTION:
    snprintf(message, sizeof(message),
             "Process %s interrupted by memory request",
             process_interrupted->name);

    break;

  case QUANTUM_TIME_INTERRUPTION:

    /// if a process was interrupted in the middle of a exec block
    if (instr_interrupted.opcode == EXEC) {
      instr_interrupted.operand = process_interrupted->segment->exec;

      /// prints information about its execution (until being interrupted)
      print_execution(process_interrupted, &instr_interrupted, SUCCESS);
    }

    snprintf(message, sizeof(message), "Process %s interrupted by quantum time",
             process_interrupted->name);

    break;

  case SEMAPHORE_INTERRUPTION:
    snprintf(message, sizeof(message), "Process %s interrupted by semaphore",
             process_interrupted->name);

    break;
  }

  append_log_message(message, PROCESS_LOG, "interrupted");
}

void print_execution(Process *process, Instruction *instruction, FLAGS flag) {
  char message[256];
  char highlight[15];

  sleep(3);

  /// for each instruction executed, prints a message with the format
  /// process <name> <opcode>
  switch (instruction->opcode) {
  case EXEC:
    snprintf(message, sizeof(message), "Process %s executed for %d seconds",
             process->name, instruction->operand);
    strcpy(highlight, "executed");

    /// finished executing a instruction, clears out the auxiliar variable
    process->segment->exec = 0;

    break;

  case P:
    if (flag == SUCCESS) {
      snprintf(message, sizeof(message), "Process %s acquired the semaphore %c",
               process->name, instruction->semaphore);
      strcpy(highlight, "acquired");
    } else {
      snprintf(message, sizeof(message),
               "Process %s is waiting for the semaphore %c", process->name,
               instruction->semaphore);
      strcpy(highlight, "waiting");
    }

    break;

  case V:
    snprintf(message, sizeof(message), "Process %s released the semaphore %c",
             process->name, instruction->semaphore);
    strcpy(highlight, "released");

    break;

  case READ:
  case WRITE:
  case PRINT:
    break;
  }

  append_log_message(message, PROCESS_LOG, highlight);
}

void print_finish(int process_id) {
  char message[256];

  sleep(3);

  snprintf(message, sizeof(message), "Process %d finished", process_id);

  append_log_message(message, PROCESS_LOG, "finished");
}

void print_syscall(SYSCALL syscall, Process *process, char semaphore_name) {
  char message[256];
  char highlight[15];
  sleep(1);

  /// for each syscall, prints a message with the format
  /// process <name> <syscall>
  switch (syscall) {
  case SEMAPHORE_SYSCALL:
    snprintf(message, sizeof(message),
             "Process %s is now the owner of semaphore %c", process->name,
             semaphore_name);
    strcpy(highlight, "owner");

    break;

  case FINISH_SYSCALL:
    snprintf(message, sizeof(message), "Process %s finished", process->name);
    strcpy(highlight, "finished");

    break;

  case MEMORY_LOAD_SYSCALL:
    snprintf(message, sizeof(message), "Memory load requisition for process %s",
             process->name);

    append_log_message(message, MEMORY_LOG, "Memory load requisition");

    return;

  case MEMORY_FINISH_SYSCALL:
    snprintf(message, sizeof(message), "Memory load finished for process %s",
             process->name);
    append_log_message(message, MEMORY_LOG, "Memory load finished");

    return;

  case CREATE_PROCESS_SYSCALL:
    snprintf(message, sizeof(message), "Process %s created", process->name);
    strcpy(highlight, "created");
    break;
  }

  append_log_message(message, PROCESS_LOG, highlight);
}

void print_scheduled(Process *process) {
  char message[256];

  sleep(1);

  snprintf(message, sizeof(message), "Process %s scheduled", process->name);

  append_log_message(message, PROCESS_LOG, "scheduled");
}
