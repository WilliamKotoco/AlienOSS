#include "../scheduler/scheduler.h"
#include <pthread.h>
#include <semaphore.h>

extern Memory *memory;

typedef enum { SUCCESS, FAILURE } FLAGS;

typedef enum {
  SEMAPHORE_INTERRUPTION,
  MEMORY_INTERRPUTION,
  NEW_PROCESS_INTERRUPTION,
  QUANTUM_TIME_INTERRUPTION
} INTERRUPTION_TYPE;

/// @brief  Initialize the CPU thread
void init_cpu();

/// @brief Emulates the CPU execution
/// @details while there is a process scheduled, the cpu calls the syscall to
/// execute the process's instructions
void cpu();

/// @brief  Emulates the execution of a given instruction
/// @param process that is executing
/// @param instruction instruction to be executed
FLAGS process_instruction(Process *process, Instruction instruction);

/// @brief the interruption of a process
///
/// Change the running process status and schedules
/// the next running process
void process_interrupt(INTERRUPTION_TYPE);

/// @brief Syscall for creating a process
void process_create_syscall(char *filename);

/// @brief  Syscall for trying the semaphore's acquisition
/// @param  Semaphore that is being queried
FLAGS semaphore_p_syscall(Semaphore *);

/// @brief  Syscall for releasing a semaphore
/// @param  Semaphore to be released
void semaphore_v_syscall(Semaphore *);

/// @brief need to add @TEMP
void memory_load_syscall();

void memory_finish_syscall();

/// @brief  Finishes running the process
/// @param filename filename
/// @details Removes the process from the PCB and calls the scheduler to forward
/// scheduling
void process_finish_syscall();

/// @brief Executes a memory load operation
/// @details Creates the pages of the process's segment and inserts them into
/// memory
/// @param process the process requisiting the operation
void memory_load_requisition();

/// @brief Adds a page into the memory's page table
/// @details Inserts a new page into the memory page table and performs the
/// swapping if there aren't enough pages free
void add_page_memory(Page *new_page);
