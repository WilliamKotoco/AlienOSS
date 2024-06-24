#ifndef DISK_H
#define DISK_H

#include "../list/list.h"
#include "../process/instruction.h"
#include "../process/process.h"

#define MAX_DISK_TRACK 200

#define DISK_OPERATION_TIME 5000
#define DISK_TRACK_MOVE_TIME 100

typedef struct disk {
  List *requests;

  int curr_track;
} Disk;

typedef struct diskRequest {
  Process *process;

  int track;

  Opcode type;

  int turnaround;
} DiskRequest;

void init_disk();

void disk();

/// @brief Creates an I/O request
/// @details Create a request and adds it into the disk request list
void create_IO_request(Process *process, Instruction *instruction);

#endif
