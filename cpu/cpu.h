#ifndef CPU_H
#define CPU_H

typedef struct memory Memory;

#include "../disk/disk.h"
#include "../log/log.h"
#include "../print/print.h"
#include "../process/instruction.h"
#include "../process/process.h"
#include "../scheduler/scheduler.h"
#include "../semaphore/semaphore.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

extern Memory *memory;

typedef enum flags { SUCCESS, FAILURE } FLAGS;

typedef enum syscall {
  FINISH_SYSCALL,
  SEMAPHORE_SYSCALL,
  MEMORY_LOAD_SYSCALL,
  MEMORY_UNLOAD_SYSCALL,
  MEMORY_FINISH_SYSCALL,
  CREATE_PROCESS_SYSCALL,
} SYSCALL;

typedef enum interruption_type {
  SEMAPHORE_INTERRUPTION,
  MEMORY_INTERRPUTION,
  NEW_PROCESS_INTERRUPTION,
  QUANTUM_TIME_INTERRUPTION,
  DISK_REQUEST_INTERRUPTION,
  DISK_FINISHED_INTERRUPTION,
  PRINT_REQUEST_INTERRUPTION,
  PRINT_FINISHED_INTERRUPTION,
} INTERRUPTION_TYPE;

///  @brief  Initializes the CPU thread
void init_cpu();

///  @brief Emulates the CPU execution
///  @details while there is a process scheduled, the cpu calls the syscall to
/// execute the process's instructions
void cpu();

///  @brief the interruption of a process
///  @param INTERRUPTION_TYPE enum that represents the interruption
///  @details Changes the running process status and reeschedules
void process_interrupt(INTERRUPTION_TYPE);

///  @brief  Syscall for creating a process
///  @param  filename name of the process' file
///  @details  calls the function to read the file and create the process'
///  struct, adds it to the PCB and the scheduler's list
void process_create_syscall(char *filename);

///  @brief  Syscall for sem_wait requisition
///  @param process requiring
///  @param  Semaphore required
///  @details identifies the desired semaphore and verifies whether it is
///  available. If so, process becomes the owner; otherwise, process is added to
///  the waiting list, with a waiting status, being interrupted
FLAGS semaphore_p_syscall(Process *, Semaphore *);

///  @brief  Syscall for sem_post requisition
///  @param  Semaphore to be released
///  @details identifies the semaphore, releasing and giving it to the first
///  process on the waiting list, if it exists
void semaphore_v_syscall(Semaphore *);

///  @brief  Finishes running the process
///  @details Removes the process from the PCB, frees its pages and calls the
///  scheduler to forward scheduling
void process_finish_syscall();

///  @brief Executes a memory load syscall
///  @details Makes the running process wait, executes a memory load
///  requisition and, after that, a memory interruption
void memory_load_syscall();

///  @brief Executes a memory load syscall
///  @details Makes the running process wait and executes a segment unload
///  requisition
void memory_unload_syscall(Process *process);

///  @brief Executes a disk requisition
///  @details Creates a disk requisition, adds it to the disk scheduler and
///  interrupts the process
///  @param process process that made the requisition
///  @param instruction instruction
void disk_requisition(Process *process, Instruction *instruction);

/// @brief Executes a print requisition
///  @details Creates a print requisition, adds it to the print list and
///  interrupts the process
/// @param process process that made the requisition
/// @param instruction instruction
void print_requisition(Process *process, Instruction *instruction);

#endif