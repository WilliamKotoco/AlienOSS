#include "main-window.h"
#include <curses.h>
#include <string.h>
#include <time.h>

extern List *PCB;
extern List *LOGS;
extern sem_t log_semaphore;

/// global windows
WINDOW *option_window;
/// secondary windows
WINDOW *process_state_window;
WINDOW *memory_state_window;
WINDOW *file_name_window;
WINDOW *memory_space_window;
WINDOW *disk_state_window;
/// @brief Load the process window, which is the window responsible to show
/// information about the processes running on the system.
/// @param  The window to be refhrased
static void load_process_window(WINDOW **win) {

  int height = getmaxy(stdscr) / 2;
  int starty = 0;
  int width = getmaxx(stdscr) *
              ((1.8 / 4)); /// FIXME: i swear to god it was the only option to
                           /// give a space between the memories window.
  int startx = getmaxx(stdscr) * 1 / 4;

  *win = newwin(height, width, starty, startx);
  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  scrollok(*win, TRUE);

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "Process window");
  wattroff(*win, A_BOLD);
  wrefresh(*win);
}

/// @brief Load the disk state window, which is the window reponsible for
/// showing the operatings happenings on disk
/// @param win
static void load_disk_state_window(WINDOW **win) {
  int height = getmaxy(stdscr) / 2;
  int starty = 0;
  int width = getmaxx(stdscr) * (1.3 / 4);
  int startx = getmaxx(stdscr) * 2.8 / 4;

  *win = newwin(height, width, starty, startx);

  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "I/O state");
  wattroff(*win, A_BOLD);

  wrefresh(*win);
}

/// @brief Load the memory space window, which is the window responsible for
/// showing the amount of space being used by the simulation's state.
/// @param win
static void load_memory_space_window(WINDOW **win) {
  int height = getmaxy(stdscr) - getmaxy(stdscr) / 2;
  int starty = 0 + getmaxy(stdscr) / 2;
  int width = getmaxx(stdscr) * (1.3 / 4);
  int startx = getmaxx(stdscr) * 2.8 / 4;

  *win = newwin(height, width, starty, startx);

  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  scrollok(*win, TRUE);

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "Memory space information");
  wattroff(*win, A_BOLD);

  wrefresh(*win);
}

/// @brief Load the memory window, which is the window responsible to show
/// information about the system memory state.
/// @param   The window do be loades
static void load_memory_window(WINDOW **win) {

  int height = getmaxy(stdscr) - getmaxy(stdscr) / 2;
  int starty = 0 + getmaxy(stdscr) / 2;
  int width = getmaxx(stdscr) *
              ((1.8 / 4)); /// FIXME: i swear to god it was the only option to
                           /// give a space between the memories window.
  int startx = getmaxx(stdscr) * 1 / 4;

  *win = newwin(height, width, starty, startx);

  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  scrollok(*win, TRUE);

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "Memory state");
  wattroff(*win, A_BOLD);

  wrefresh(*win);
}

static int load_file_name_window(WINDOW **win) {
  char filename[245];

  *win = newwin(getmaxy(stdscr) * 1 / 4, getmaxx(stdscr) / 4,
                getmaxy(stdscr) - (getmaxy(stdscr) * 1 / 4), 0);

  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "File");
  wattroff(*win, A_BOLD);

  mvwprintw(*win, 2, 2, "q to exit");
  mvwprintw(*win, 5, 2, "Insert file name: ");
  echo();
  wgetnstr(*win, filename, sizeof(filename) - 1);
  noecho();

  if (filename[0] == 'q')
    return 0;
  process_create_syscall(filename);

  wrefresh(*win);
  return 1;
}

/// @brief  Prints the specific log message
/// @param win  window to be forwared the message
/// @param log the message to be print
/// @param highlight the subtring of the message that will be hilighted
/// @param pos position in the y axis of the window
static void print_log(WINDOW *win, char *log, char *highlight, int pos) {
  char *position;   /// stores the position of highlight within the log message
  char buffer[256]; /// buffer to store substrings in the process of dividing
                    /// the strings

  position = strstr(log, highlight);

  /// stores the lenght of the substring before the word
  int len_before = position - log;

  strncpy(buffer, log, len_before);
  buffer[len_before] = '\0';

  wattron(win, COLOR_PAIR(2));
  mvwprintw(win, pos, 8, buffer);
  wattroff(win, COLOR_PAIR(2));

  wattron(win, COLOR_PAIR(4));
  wattron(win, A_BOLD);
  wprintw(win, highlight);
  wattroff(win, COLOR_PAIR(4));
  wattroff(win, A_BOLD);

  char *position_after_highlight = position + strlen(highlight);
  char *final_position = log + strlen(log);

  if (position_after_highlight < final_position) {
    wattron(win, COLOR_PAIR(2));
    wprintw(win, "%s", position_after_highlight);
    wattroff(win, COLOR_PAIR(2));
  }

  wrefresh(win);
}

void show_and_run() {
  initscr();
  cbreak();

  start_color();
  init_pair(1, COLOR_BLACK, COLOR_BLACK); /// background color
  init_pair(2, COLOR_GREEN, COLOR_BLACK); /// Alien green color
  init_pair(3, COLOR_GREEN, COLOR_WHITE); /// Color for inputting from user
  init_pair(4, COLOR_WHITE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));

  load_process_window(&process_state_window);

  load_memory_window(&memory_state_window);

  load_memory_space_window(&memory_space_window);

  load_disk_state_window(&disk_state_window);
  /// get the maximum from the standard screen
  option_window = newwin(getmaxy(stdscr) * 3 / 4, getmaxx(stdscr) / 4, 0, 0);

  /// colloring the window
  wbkgd(option_window, COLOR_PAIR(1));
  wattron(option_window, COLOR_PAIR(2));

  display_ascii_art(option_window);

  box(option_window, 0, 0);

  wrefresh(option_window);
  while (load_file_name_window(&file_name_window))
    ;
  delwin(memory_state_window);
  delwin(process_state_window);
  delwin(option_window);
  delwin(file_name_window);
  endwin();

  endwin();
}

void refresh_log(char *highlight) {
  static int start_processy =
      2; /// current position in the y axis in the process
  static int start_memoryy = 2; /// current position on the y axis in the memory
  static int start_memory_spacey =
      2; /// current position in the y axis in the mermoy space.

  static int start_disk_spacey =
      2; /// current position in the y axis in the disk window
  Node *current_log = LOGS->tail;

  LogMessage *curr = (LogMessage *)current_log->data;

  switch (curr->log_type) {

  case PROCESS_LOG:

    if (start_processy == getmaxy(stdscr) / 2) {
      delwin(process_state_window);
      load_process_window(&process_state_window);
      start_processy = 2;
    }
    print_log(process_state_window, curr->log_message, highlight,
              start_processy);
    start_processy++;
    break;

  case MEMORY_LOG:

    if (start_memoryy == getmaxy(stdscr) / 2) {
      delwin(memory_state_window);
      load_memory_window(&memory_state_window);
      start_memoryy = 2;
    }

    print_log(memory_state_window, curr->log_message, highlight, start_memoryy);

    start_memoryy++;
    break;

  case MEMORY_SPACE_LOG:

    if (start_memory_spacey == 2 + LINES_SPACE_LOG) {
      delwin(memory_space_window);
      load_memory_space_window(&memory_space_window);
      start_memory_spacey = 2;
    }

    print_log(memory_space_window, curr->log_message, highlight,
              start_memory_spacey);

    start_memory_spacey++; /// space information overlaps
    break;

  case DISK_LOG:

    if (start_disk_spacey == getmaxy(stdscr) / 2) {
      delwin(disk_state_window);
      load_disk_state_window(&disk_state_window);
      start_disk_spacey = 2;
    }

    print_log(disk_state_window, curr->log_message, highlight,
              start_disk_spacey);

    start_disk_spacey++;
    break;
  default:
  }
}
