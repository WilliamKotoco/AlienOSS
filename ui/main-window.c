#include "main-window.h"
#include "utils.h"
#include <curses.h>
#include <ncurses.h>

void show_and_run() {

  WINDOW *option_window;

  initscr();
  cbreak();

  start_color();
  init_pair(1, COLOR_BLACK, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));

  refresh();

  option_window = newwin(100, 50, 0, 0);

  /// colloring the window
  wbkgd(option_window, COLOR_PAIR(1));
  wattron(option_window, COLOR_PAIR(2));

  display_ascii_art(option_window);

  box(option_window, 0, 0);

  /// Options inside the option_window
  keypad(option_window, TRUE);
  char options[3][100] = {"Create a new process",
                          "Toggle semaphore aquisition (?)", "Exit"};
  int choice;
  int highlight = 0;

  while (1) {
    for (int i = 0; i < 3; i++) {
      if (highlight == i) {
        wattron(option_window, A_REVERSE);
      }
      mvwprintw(option_window, 30 + i + 1, 1, options[i]);
      wattroff(option_window, A_REVERSE);
    }
    choice = wgetch(option_window);

    switch (choice) {

    case KEY_UP:
      highlight--;
      if (highlight == -1)
        highlight = 0;
      break;

    case KEY_DOWN:
      highlight++;
      if (highlight == 3)
        highlight = 2;
      break;

      /// Enter key
    case '\n':
      if (highlight == 0) {
        mvwprintw(option_window, 35, 1, "Created"); /// temporary

      } else if (highlight == 1) {
        mvwprintw(option_window, 35, 1, "Toggled"); /// temporary

      } else {
        return;
      }
    default:
      break;
    }
  }

  wrefresh(option_window);

  endwin();
}
