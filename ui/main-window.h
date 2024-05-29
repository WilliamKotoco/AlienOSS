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

/// @brief Prints the message on the log screen, highlithing
/// @param highlight string to be highlited
///
/// @details Reads the last log message and move the message to the respective
/// window based on the `log_type`
void refresh_log(char *highlight);

#endif
