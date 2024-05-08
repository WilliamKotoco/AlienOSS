#include <stdio.h>
#include <stdlib.h>
#include "../process/process.h"
#include "../list/list.h"

#define MEMORY_SIZE 1073742000
#define PAGE_SIZE 8192
#define NUM_PAGES 131072

/// a memory page, which contains 
typedef struct page{
    int number;
    int segment_id;
    int process_id;
    int reference_bit;
} Page;

typedef struct memory{
    List *pages;
    int num_free_pages;
} Memory;

typedef struct segment{
    int id;
    int size;
    int num_pages;
    int in_memory;
    List *instructions;
} Segment;