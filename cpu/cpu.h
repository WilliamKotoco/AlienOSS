#include <pthread.h>
#include <semaphore.h>

/// @brief  Initialize the CPU thread
void init_cpu();

/// @brief Emulates the CPU execution
/// @details while there is a process scheduled, the cpu calls the syscall to execute the process's instructions
void cpu();

/// @brief Fetches the next instruction
/// @return number of the page that contains the instruction
int fetch_instruction(int PC);

/// @brief  Function responsible for emulating CPU's execution
/// @details This function execute the instructions of the current
/// running process in the scheduler. It uses a semaphore to
/// verify if the running process is not being changed by the scheduler.
void execute();