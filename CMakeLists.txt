cmake_minimum_required(VERSION 3.6)




set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -lncurses")

project(AlienOS C)


add_executable(AlienOS 
  main.c
  ui/main-window.h
  ui/main-window.c
  ui/utils.h 
  ui/utils.c
  )


target_link_libraries(AlienOS ${CURSES_LIBRARIES})
