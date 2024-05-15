#include "cpu.h"
#include "../list/list.h"
#include "../scheduler/scheduler.h"

/// Necessary for global variables
extern Scheduler *scheduler;
extern sem_t process_semaphore;
extern Memory *memory;
extern List *PCB;
void init_cpu() {
  pthread_t cpu_id;

  pthread_attr_t cpu_attr;

  pthread_attr_init(&cpu_attr);

  pthread_attr_setscope(&cpu_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&cpu_id, NULL, (void *)execute, NULL);
}

void cpu() {
  while (1) {                          /// constantly running
    if (!scheduler->running_process) { /// no running process
      forward_scheduling();            /// schedules another process
    } else {                           /// there is a scheduled process already
      while (scheduler->running_process &&
             scheduler->running_process->PC <
                 scheduler->running_process->segment->num_instructions &&
             scheduler->running_process->remaining_time >
                 0) { /// while the process is still running

        sem_wait(&process_semaphore);

        Process *running = scheduler->running_process;

        if (!running->segment
                 ->present_bit) { // segment is not loaded in the memory
          // chamar função de carregar, que chama interrupção e tira o processo
          // de executar
        } else {
          running->segment->used_bit = 1;
          // procura página na tabela de páginas e muda bit

          process_instruction(
              running,
              running->segment
                  ->instructions[running->PC]); // aumenta PC aqui se der certo

        }

        sem_post(&process_semaphore);
      }

      /// exited while, that means the process stopped running, either because
      /// it was interrupted, because it has finished running, or because it
      /// used all of its quantum time

      if (scheduler->running_process->PC >=
          scheduler->running_process->segment->num_instructions) { /// finished

        process_finish_syscall(); // process finished
      }

      if (scheduler->running_process->remaining_time <=
          0) { // completed the quantum time
        process_interrupt_syscall();
      }
    }
  }
}



void process_instruction(Process *process, Instruction instruction) {
  switch (instruction.opcode) {
  /// EXEC has the format EXEC X, where X is the necessary time to execute  a
  /// given instruction
  case EXEC:
    process->remaining_time -= instruction.operand;
    /// updates program counter
    process->PC++;
    break;

    /// later
  case READ:
    break;

  /// later
  case WRITE:
    break;

  case P:
    char semaphore_id = instruction.semaphore;
    Node *semaphore_node = find(memory->semaphores, &semaphore_id);

    Semaphore *semaphore = (Semaphore *)semaphore_node;
    FLAGS result = semaphore_p_syscall(semaphore);

    if (result == SUCCESS)
    {          
      scheduler->running_process->remaining_time -=
        200; /// 200 time required to semaphore
        scheduler->running_process->PC++;
    }
    else
    {
      process_interrupt_syscall();
    }

    break;

  case V:

    char semaphore_id = instruction.semaphore;
    Node *semaphore_node = find(memory->semaphores, &semaphore_id);

    Semaphore *semaphore = (Semaphore *)semaphore_node;
    semaphore_v_syscall(semaphore);

    break;

  /// later
  case PRINT:
    break;

  default:
    printf("Error: the opcode is invalid");
    exit(0);
  }
}

void process_interrupt_syscall() {
  scheduler->running_process->status = WAITING;

  /// re-schedules
  forward_scheduling();
}

FLAGS semaphore_p_syscall(Semaphore *semaphore) {
  /// if there are no elements in the semaphore's queue
  if (semaphore->owner_id == -1) {
    semaphore->owner_id = scheduler->running_process->id;
    return SUCCESS;
  }
  /// enqueue the process if there are elements in the queue
  
    push(semaphore->processes_waiting, scheduler->running_process);
  return FAILURE;
}

void semaphore_v_syscall(Semaphore *semaphore) {
  semaphore->owner_id = -1;

  Process *new_process = (Process *)pop(semaphore->processes_waiting);

  if (new_process) {
    new_process->status = READY;

    add_process_scheduler(new_process);

    semaphore->owner_id = new_process->id;
  }

  forward_scheduling();
}

void process_finish_syscall() {
  /// delete the scheduler from the PCB
  delete_list(PCB, scheduler->running_process);

  /// calls the scheduler to advance scheduling
  forward_scheduling();
}

void memory_load_syscall() {
  scheduler->running_process->status = WAITING;
  /// calls here function to load the memory
  /// func
  ///

  /// change process status and calls forward_scheduling to remove it
  /// and schedule the next running process
  scheduler->running_process->status = READY;
  forward_schedulin();
}

void process_create_syscall(char * filename) {

   Process *new_process = create_process(filename);

  /// @TEMP não entendi se a gente vai passar o nome do arquivo aqui
  /// e chamar a create process sem arquivo ou se nem chama a create process

  add_process_scheduler(new_process);
  /// creating a new process interrupts the current one.
  process_interrupt_syscall();
}
void memory_load_requisition(Process *process){
  if(memory->num_free_pages >= process->segment->num_pages){ /// there is space available in the memory for the pages
    // creating the pages and inserting them into memory's page table
    for(int i = 0; i < process->segment->num_pages; i++){
      Page *new_page = malloc(sizeof(Page));
      new_page->process_id = process->id;
      new_page->segment_id = process->segment->id;

      add_page_memory(new_page);
    }
  } else{
    /// second chance
  }
}

void add_page_memory(Page *new_page){ // percorrer páginas e adicionar se achar livre no meio. Se não achar, adicionar depois
  int i = 0;

  while(memory->pages[i].free == 0 && i < NUM_PAGES){ /// searches first free page
    i++;
  }

  memory->pages[i].number = new_page->number;
  memory->pages[i].process_id = new_page->process_id;
  memory->pages[i].segment_id = new_page->segment_id;
  memory->pages[i].free = 0;

  memory->num_free_pages--;
}
