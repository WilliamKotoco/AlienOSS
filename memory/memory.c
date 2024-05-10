#include "memory.h"
/// Necessary for using global variables defined in main.c
extern Memory *memory;

int compare_pages(void *d1, void *d2) {
  Page *p1 = (Page *)d1;

  int d11 = p1->number;

  int *d22 = (int *)d2;

  if (d11 == *d22) {
    return 1;
  } else {
    return 0;
  }
}