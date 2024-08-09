## Compilar
Para compilar o projeto, é necessário possuir instalados: gcc, make e ncurses

Caso não possua ncurses, o comando para instalar em distribuições Ubuntu é:
sudo apt-get install libncurses5-dev libncursesw5-dev

## Rodar
Para rodar o projeto, basta executar os seguintes comandos:

```
cmake .
make 
./AlienOs
```


Caso não dê certo, copiar e colar o seguinte comando para compilar: 
```
gcc -g -pthread -Wall  \
  main.c \
  ui/main-window.h ui/main-window.c \
  ui/utils.h ui/utils.c \
  memory/memory.h memory/memory.c \
  process/instruction.h process/instruction.c process/process.h process/process.c \
  semaphore/semaphore.h semaphore/semaphore.c \
  list/list.h list/list.c \
  cpu/cpu.h cpu/cpu.c \
  scheduler/scheduler.h scheduler/scheduler.c \
  log/log.h log/log.c \
  disk/disk.h disk/disk.c \
  print/print.h print/print.c -o AlienOS -lncurses
```
