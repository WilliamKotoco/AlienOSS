#include "cpu.h"
#include "../scheduler/scheduler.h"

/// Necessary for global variables
extern Scheduler *scheduler;
extern sem_t process_semaphore;

void init_cpu() {
  pthread_t cpu_id;

  pthread_attr_t cpu_attr;

  pthread_attr_init(&cpu_attr);

  pthread_attr_setscope(&cpu_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&cpu_id, NULL, (void *)execute, NULL);
}

void cpu() {
  while (1) { /// constantly running
    if (! scheduler->running_process) { /// no running process
      forward_scheduling(); /// schedules another process
    } else{ /// there is a scheduled process already
      while (scheduler->running_process && scheduler->running_process->PC 
        < scheduler->running_process->segment->num_instructions &&
        scheduler->running_process->remaining_time > 0) { /// while the process is still running
        
        sem_wait(&process_semaphore);

        Process *running = scheduler->running_process;

        if(! running->segment->present_bit){ // segment is not loaded in the memory 
          // chamar função de carregar, que chama interrupção e tira o processo de executar
        } else{
          int page_number = fetch_instruction(running->PC);
          // procura página na tabela de páginas e muda bit

          execute_instruction(running, running->segment->instructions[running->PC]); // aumenta PC aqui se der certo
        }
        
        sem_post(&process_semaphore);
      }

      /// exited while, that means the process stopped running, either because it was interrupted, 
      /// because it has finished running, or because it used all of its quantum time

      if(scheduler->running_process->PC >= scheduler->running_process->segment->num_instructions){ /// finished
        syscall(PROCESS_FINISH, scheduler->running_process); // process finished
      } 
      
      if(scheduler->running_process->remaining_time <= 0){ // completed the quantum time
        syscall(PROCESS_INTERRUPT, QUANTUM_COMPLETED);
      }  
    }
  }
}

int fetch_instruction(int PC){
  return PC / PAGE_SIZE; /// the number of the page where the instruction is
}