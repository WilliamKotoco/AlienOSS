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

/// @brief initializes the disk structure and creates the disk thread
void init_disk();

/// @brief Emulates the disk operation using the SCAN algorithm to serve
/// requests
/// @details whenever there is a requisition pending on the disk's list, it
/// performs a scan, serving the requests in ascending order of the tracks. When
/// fulfilling a request, it prints a run message, changes the process status to
/// ready, and creates an interrupt
void disk();

/// @brief Creates an I/O request
/// @details Create a request and adds it into the disk request list
void create_IO_request(Process *process, Instruction *instruction);

#endif
