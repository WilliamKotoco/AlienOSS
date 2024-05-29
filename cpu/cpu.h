#ifndef CPU_H
#define CPU_H
#include "../log/log.h"
#include "../scheduler/scheduler.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

extern Memory *memory;

typedef enum flags { SUCCESS, FAILURE } FLAGS;

typedef enum syscall {
  FINISH_SYSCALL,
  P_SYSCALL,
  V_SYSCALL,
  MEMORY_LOAD_SYSCALL,
  MEMORY_FINISH_SYSCALL,
  CREATE_PROCESS_SYSCALL
} SYSCALL;

typedef enum interruption_type {
  SEMAPHORE_INTERRUPTION,
  MEMORY_INTERRPUTION,
  NEW_PROCESS_INTERRUPTION,
  QUANTUM_TIME_INTERRUPTION
} INTERRUPTION_TYPE;

///  @brief  Initializes the CPU thread
void init_cpu();

///  @brief Emulates the CPU execution
///  @details while there is a process scheduled, the cpu calls the syscall to
/// execute the process's instructions
void cpu();

///  @brief  Emulates the execution of a given instruction
///  @param process process being executed
///  @param instruction instruction to be executed
///
///  @details identifies the given instruction and calls the specific syscall
void process_instruction(Process *process, Instruction *instruction);

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

void memory_load_syscall();

void memory_finish_syscall();

///  @brief  Finishes running the process
///  @details Removes the process from the PCB, frees its pages and calls the
///  scheduler to forward scheduling
void process_finish_syscall();

///  @brief Executes a memory load operation
///  @param process the process requisiting the operation
///  @details Creates the pages of the process's segment and inserts them into
/// memory
void memory_load_requisition(Process *);

///  @brief Loads a process' segment into memory
///  @param process given process
void load_segment(Process *);

///  @brief Adds a page into the memory's page table
///  @details Inserts a new page into the memory page table and performs the
/// swapping if there aren't enough pages free
void add_page_memory(Page *new_page);

///  @brief Unloads a process' segment
///  @param segment segment to be unloaded
///  @details removes each of the segment's pages and updates the segment's bits
void memory_unload_segment(Segment *segment);

///  @brief deletes a page of the memory
///  @param id of the owner process
///  @details finds the first used page of the process and frees it
void memory_delete_page(int id);

///  @brief swaps a segment
///  @details uses the second chance algorithm to choose the segment being
///  swapped
void swap_segment();

/// @brief update global variable using a mutex
/// @param val  true or false
void update_new_process_flag(bool val);

#endif