#include "../scheduler/scheduler.h"
#include <pthread.h>
#include <semaphore.h>


typedef enum {SUCCESS, FAILURE} FLAGS;
/// @brief  Initialize the CPU thread
void init_cpu();

/// @brief Emulates the CPU execution
/// @details while there is a process scheduled, the cpu calls the syscall to
/// execute the process's instructions
void cpu();

/// @brief Fetches the next instruction
/// @return number of the page that contains the instruction
int fetch_instruction(int PC);

/// @brief  Function responsible for emulating CPU's execution
/// @details This function execute the instructions of the current
/// running process in the scheduler. It uses a semaphore to
/// verify if the running process is not being changed by the scheduler.
void execute();

/// @brief  Emulates the execution of a given instruction
/// @param process that is executing
/// @param instruction instruction to be executed
void process_instruction(Process *process, Instruction instruction);

/// @brief Syscall for the interruption of a process
///
/// Change the running process status and schedules
/// the next running process
void process_interrupt_syscall();

/// @brief Syscall for creating a process
void process_create_syscall();

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
void process_finish_syscall(char *filename);