
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

/// \brief This functions reads a file with an ASCII draw and put it into
/// ncurses
///
/// Go through each line and prints it inside the window
void display_ascii_art(WINDOW *win);
