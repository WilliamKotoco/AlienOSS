#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include "../cpu/cpu.h"
#include "utils.h"
#include <ncurses.h>

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

/// @brief  Returns the file name to load a new process
/// @return The file name
/// @details It creates a new window to receive the input string from the user.
char *get_process_filename();
