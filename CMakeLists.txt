cmake_minimum_required(VERSION 3.12)




set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pthread -Wall -lncurses")

project(AlienOS C)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(AlienOS 
  main.c
  ui/main-window.h
  ui/main-window.c
  ui/utils.h 
  ui/utils.c
  memory/memory.h
  memory/memory.c
  process/instruction.h
  process/instruction.c
  process/process.h
  process/process.c
  semaphore/semaphore.h
  semaphore/semaphore.c
  list/list.h 
  list/list.c
  cpu/cpu.h 
  cpu/cpu.c 
  scheduler/scheduler.h 
  scheduler/scheduler.c 
  log/log.h
  log/log.c 
  )


target_link_libraries(AlienOS ${CURSES_LIBRARIES})
