#include <stdio.h> 
#include <stdlib.h> 
#include "../process/process.h"  

typedef struct scheduler{   
  Process *running_process;   
  List *ready_processes; 
} scheduler;
