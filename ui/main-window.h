#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "../log/log.h"
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

/// @brief Prints the message on the log screen, highlithing
/// @param highlight string to be highlited
///
/// @details Reads the last log message and move the message to the respective
/// window based on the `log_type`
void refresh_log(char *highlight);

/// @brief Get the file name from the user
/// @param win
void load_file_name_window(WINDOW **win);

/// @brief  Prints the specific log message
/// @param win  window to be forwared the message
/// @param log the message to be print
/// @param highlight the subtring of the message that will be hilighted
/// @param pos position in the y axis of the window
void print_log(WINDOW *win, char *log, char *highlight, int pos);

#endif
