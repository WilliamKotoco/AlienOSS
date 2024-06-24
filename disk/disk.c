#include "disk.h"

extern Disk *disk_scheduler;
extern sem_t disk_semaphore;

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

void disk() {
  while (1) {
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
  DiskRequest *disk_request = malloc(sizeof(DiskRequest));
  disk_request->process = process;
  disk_request->type = instruction->opcode;
  disk_request->track = instruction->operand;

  sem_wait(&disk_semaphore);
  add_disk_request(disk_request);
  sem_post(&disk_semaphore);
}