#include <pthread.h>
#include <semaphore.h>

/// @brief  Initialize the CPU thread
void init_cpu();

/// @brief  Function responsible for emulating CPU's execution
/// @details This function execute the instructions of the current
/// running process in the scheduler. It has uses a semaphore to
/// verify if the running process is not being changed by the scheduler.
void execute();