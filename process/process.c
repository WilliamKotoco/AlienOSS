#include "process.h"

int compare_processes(void *d1, void *d2) {
  Process *p1 = (Process *)d1;

  int d11 = p1->id;

  int *d22 = (int *)d2;

  if (d11 == *d22) {
    return 1;
  } else {
    return 0;
  }
}

void create_process(char *file_name){
    
}