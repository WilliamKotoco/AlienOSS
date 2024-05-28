#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "../cpu/cpu.h"
#include "utils.h"
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#define ALIEN_GREEN 11

/// \brief Window for displaying information and to user
///
/// @details Display the logo and the options to create a process, display
/// memory and display idk
void show_and_run();

/// @brief Load the process window, which is the window responsible to show
/// information about the processes running on the system.
/// @param  The window to be refhrased
void load_process_window(WINDOW **);

/// @brief Load the memory window, which is the window responsible to show
/// information about the system memory state.
/// @param   The window do be loades
void load_memory_window(WINDOW **);
/// @brief  Returns the file name to load a new process
/// @return The file name
/// @details It creates a new window to receive the input string from the user.
char *get_process_filename();

/// @brief Initializes the log thread
void init_log();

/// Thread sleeps until receive information of new log message available
void refresh_log();

void load_file_name_window(WINDOW **win);

#endif
