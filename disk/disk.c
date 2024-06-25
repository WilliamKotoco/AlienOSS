#include "disk.h"
#include "../log/log.h"
#include "../ui/main-window.h"

extern Disk *disk_scheduler;
extern sem_t disk_semaphore;

extern Scheduler *scheduler;
extern sem_t scheduler_semaphore;

extern sem_t interrupt_semaphore;

static int compare_disk_resquests(void *d1, void *d2) {
  DiskRequest *p1 = (DiskRequest *)d1;

  int d11 = p1->track;

  int *d22 = (int *)d2;
  int d23 = *d22;

  if (d11 == d23) {
    return 1;
  } else {
    return 0;
  }
}

void init_disk() {
  disk_scheduler = malloc(sizeof(Disk));

  disk_scheduler->curr_track = 0;
  disk_scheduler->requests =
      create_list(sizeof(DiskRequest), compare_disk_resquests);

  pthread_t disk_id;
  pthread_attr_t disk_attr;

  pthread_attr_init(&disk_attr);
  pthread_attr_setscope(&disk_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&disk_id, NULL, (void *)disk, NULL);
}

/// @brief Finishes a disk request
/// @param request the request being finished
/// @details changes the status of the process that made the request to ready
/// and reinserts it to the scheduler's list
static void disk_request_finished(DiskRequest *request) {
  request->process->status = READY;

  sem_wait(&scheduler_semaphore); // mais de uma thread muda a lista
  add_process_scheduler(request->process);
  sem_post(&scheduler_semaphore);
}

/// @brief Fulfills a disk request
/// @param request the request being fulfilled
/// @details Prints the disk execution, deletes the request from the list,
/// finishes it and calls an interrupt
static void fulfill_disk_request(DiskRequest *request) {
  /// prints the execution
  print_disk_execution(request->process, request->type, request->track);

  /// finished the request
  disk_request_finished(request);

  /// deletes the request from the list, as it has been fulfilled
  delete_list(disk_scheduler->requests, &request->track);

  /// interrupts the current process on the CPU
  sem_wait(&interrupt_semaphore);
  process_interrupt(DISK_FINISHED_INTERRUPTION);
  sem_post(&interrupt_semaphore);
}

/// @brief Performs a disk scan
/// @details Scroll through the list of requests in ascending order of tracks,
/// attending them
static void disk_sweep() {
  disk_scheduler->curr_track = 0;

  Node *request_fulfilled = disk_scheduler->requests->header;
  DiskRequest *request_fulfilled_data;

  /// for all requests
  while (request_fulfilled && disk_scheduler->curr_track < MAX_DISK_TRACK) {
    request_fulfilled_data = (DiskRequest *)request_fulfilled->data;
    request_fulfilled = request_fulfilled->next;

    /// calculates the travelled distance and time necessary
    int track_distance =
        request_fulfilled_data->track - disk_scheduler->curr_track;
    int time = track_distance * DISK_TRACK_MOVE_TIME + DISK_OPERATION_TIME;

    sleep(time / 1000);

    disk_scheduler->curr_track = request_fulfilled_data->track;

    /// fulfills the request
    sem_wait(&disk_semaphore);
    fulfill_disk_request(request_fulfilled_data);
    sem_post(&disk_semaphore);
  }
}

void disk() {
  while (1) {
    /// there is a pending request
    if (disk_scheduler->requests->header) {
      disk_sweep();
    }
  }
}

static void add_disk_request(DiskRequest *new_request) {
  /// alocates space
  Node *new_node = malloc(sizeof(Node));
  new_node->data = new_request;
  new_node->prev = new_node->next = NULL;

  /// empty list
  if (!disk_scheduler->requests->header) {
    disk_scheduler->requests->header = new_node;
    disk_scheduler->requests->tail = new_node;

    return;
  }

  DiskRequest *header = (DiskRequest *)disk_scheduler->requests->header->data;

  /// new process has the smallest track
  if (new_request->track < header->track) {
    new_node->next = disk_scheduler->requests->header;
    disk_scheduler->requests->header = new_node;

    return;
  }

  /// search for the next process with a smaller track
  Node *aux = disk_scheduler->requests->header;
  DiskRequest *next_request;

  while (aux->next) {
    next_request = (DiskRequest *)aux->next->data;

    /// first process with a bigger track number
    if (next_request->track > new_request->track) {
      break;
    }

    aux = aux->next;
  }

  new_node->next = aux->next;
  new_node->prev = aux;

  aux->next = new_node;

  if (!new_node->next) { /// new node is the last
    disk_scheduler->requests->tail = new_node;
  } else {
    aux->next->prev = new_node;
  }
}

void create_IO_request(Process *process, Instruction *instruction) {
  DiskRequest *disk_request;

  disk_request = malloc(sizeof(DiskRequest));
  disk_request->process = process;
  disk_request->type = instruction->opcode;
  disk_request->track = instruction->operand;

  /// inserts the request into the disk's list of request
  sem_wait(&disk_semaphore);
  add_disk_request(disk_request);
  sem_post(&disk_semaphore);
}
