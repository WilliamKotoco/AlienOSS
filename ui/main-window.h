#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#define ALIEN_GREEN 11

/// \brief Window for displaying information and to user
///
/// Display the logo and the options to create a process, display memory and
/// display idk
void show_and_run();

void load_process_window(WINDOW **);

char *get_process_filename();
