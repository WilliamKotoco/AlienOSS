#include <stdio.h>
#include <stdlib.h>
#include "../list/list.h"

/// stores information about a semaphore, which has a owner and can be shared among different processes
typedef struct semaphore{
    char *name; //!< semaphore name
    List *processes_waiting; ///< processes waiting to acquire the ownership of this semaphore
    int owner_id; //!< current owner of this semaphore
} Semaphore;