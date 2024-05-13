#include <pthread.h>
#include <semaphore.h>
#include "../process/process.h"

extern Memory *memory;

/// @brief  Initialize the CPU thread
void init_cpu();

/// @brief Emulates the CPU execution
/// @details while there is a process scheduled, the cpu calls the syscall to execute the process's instructions
void cpu();

/// @brief  Function responsible for emulating CPU's execution
/// @details This function execute the instructions of the current
/// running process in the scheduler. It uses a semaphore to
/// verify if the running process is not being changed by the scheduler.
void execute();

/// @brief Executes a memory load operation
/// @details Creates the pages of the process's segment and inserts them into memory
/// @param process the process requisiting the operation
void memory_load_requisition(Process *process);

/// @brief Adds a page into the memory's page table
/// @details Inserts a new page into the memory page table and performs the swapping if there aren't enough pages free
void add_page_memory(Page *new_page);