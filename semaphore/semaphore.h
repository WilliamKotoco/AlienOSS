#include "../list/list.h"
#include <stdio.h>
#include <stdlib.h>

/// stores information about a semaphore, which has a owner and can be shared
/// among different processes
typedef struct semaphore {
  char name;               //!< semaphore name
  List *processes_waiting; ///< processes waiting to acquire the ownership of
                           ///< this semaphore
  int owner_id;            //!< current owner of this semaphore
} Semaphore;

/// Compares two semaphores based on their name. Used on the generic list.
/// @param d1 a semaphore
/// @param d2 the semaphore being compared to d1
/// @return 1 if d1 and d2 are the same semaphore, and 0 otherwise
int compare_semaphores(void *d1, void *d2);