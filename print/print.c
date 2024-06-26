#include "print.h"

extern List *print_requests;
extern sem_t print_semaphore;
extern sem_t scheduler_semaphore;
extern int prints_id;
extern sem_t interrupt_semaphore;

/// @brief Compares two print requests
/// @param d1 the first print request
/// @param d2 the id of the second print request
/// @return 1 if both rquests are the same and 0 otherwise
static int compare_print_resquests(void *d1, void *d2) {
  PrintRequest *p1 = (PrintRequest *)d1;

  int d11 = p1->id;

  int *d22 = (int *)d2;
  int d23 = *d22;

  if (d11 == d23) {
    return 1;
  } else {
    return 0;
  }
}

void init_print() {
  print_requests = create_list(sizeof(PrintRequest), compare_print_resquests);

  pthread_t print_id;
  pthread_attr_t print_attr;

  pthread_attr_init(&print_attr);
  pthread_attr_setscope(&print_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&print_id, NULL, (void *)print, NULL);
}

void create_print_request(Process *process, Instruction *instruction) {
  PrintRequest *print_request;

  print_request = malloc(sizeof(PrintRequest));
  print_request->id = prints_id++;
  print_request->process = process;
  print_request->time = instruction->operand;

  /// inserts the request into the list of requests
  sem_wait(&print_semaphore);
  push(print_requests, print_request);
  sem_post(&print_semaphore);
}

/// @brief Finishes a print request
/// @param request the request being finished
/// @details makes the process that made the request ready and inserts it again
/// into the scheduler's list
static void print_request_finished(PrintRequest *request) {
  request->process->status = READY;

  sem_wait(&scheduler_semaphore); // mais de uma thread muda a lista
  add_process_scheduler(request->process);
  sem_post(&scheduler_semaphore);
}

/// @brief Fulfills a print request
/// @details Prints it execution, finishes it and interrupts the current process
static void fulfill_print() {
  Node *request_fulfilled = print_requests->header;
  PrintRequest *request_fulfilled_data =
      (PrintRequest *)request_fulfilled->data;

  // sleep(request_fulfilled_data->time / 1000);

  /// prints the execution
  print_print_execution(request_fulfilled_data->process,
                        request_fulfilled_data->time);

  /// finished the request
  print_request_finished(request_fulfilled_data);

  /// deletes the request from the list, as it has been fulfilled
  sem_wait(&print_semaphore);
  delete_list(print_requests, &request_fulfilled_data->id);
  sem_post(&print_semaphore);

  /// interrupts the current process on the CPU
  sem_wait(&interrupt_semaphore);
  process_interrupt(PRINT_FINISHED_INTERRUPTION);
  sem_post(&interrupt_semaphore);
}

void print() {
  while (1) {
    /// there is a pending request
    if (print_requests->header) {
      fulfill_print();
    }
  }
}