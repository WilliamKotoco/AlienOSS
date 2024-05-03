#include "utils.h"
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
void display_ascii_art(WINDOW *win) {
  FILE *file = fopen("assets/alien.txt", "r");

  if (!file) {
    // tmp
    printf("not found the image");
    exit(1);
  }

  char line[100000];

  int y = 5;
  while (fgets(line, sizeof(line), file)) {
    mvwprintw(win, y, 1, line);
    y++;
  }
}
