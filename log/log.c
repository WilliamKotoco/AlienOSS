#include "log.h"
#include <semaphore.h>
#include <string.h>
extern List *LOGS;
extern sem_t log_semaphore;

void append_log_message(char *message, LOG_TYPE log_type) {
  LogMessage *log_message = malloc(sizeof(LogMessage));

  strcpy(log_message->log_message, message);
  log_message->log_type = log_type;

  push(LOGS, log_message);

  int sem_value;
  sem_getvalue(&log_semaphore, &sem_value);

  /// wake up the thread responsible for printing new messages
  sem_post(&log_semaphore);
}

int compare_log(void *d1, void *d2) { return 1; }
