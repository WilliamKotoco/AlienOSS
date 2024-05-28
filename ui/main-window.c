#include "main-window.h"
#include <curses.h>
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
Node *current_log;

void show_and_run() {
  initscr();
  cbreak();

  start_color();
  init_pair(1, COLOR_BLACK, COLOR_BLACK); /// background color
  init_pair(2, COLOR_GREEN, COLOR_BLACK); /// Alien green color
  init_pair(3, COLOR_GREEN, COLOR_WHITE); /// Color for inputting from user
  bkgd(COLOR_PAIR(1));

  load_process_window(&process_state_window);
  /// @FIX: it is impossible to print the log header within the LOG thread
  /// without the ncurses exploding and the reason is, as the beach boys said:
  /// god only knows
  mvwprintw(process_state_window, 2, 8, "Process 0 created");
  
  load_memory_window(&memory_state_window);

  //  load_memory_window(memory_state_window);
  /// get the maximum from the standard screen
  option_window = newwin(getmaxy(stdscr) * 3 / 4, getmaxx(stdscr) / 4, 0, 0);

  /// colloring the window
  wbkgd(option_window, COLOR_PAIR(1));
  wattron(option_window, COLOR_PAIR(2));

  display_ascii_art(option_window);

  box(option_window, 0, 0);

  wrefresh(option_window);
  while (1) {
    load_file_name_window(&file_name_window);
  }

  endwin();
}

void load_process_window(WINDOW **win) {
  *win = newwin(getmaxy(stdscr) / 2, getmaxx(stdscr) * (3 / 4), 0,
                getmaxx(stdscr) * 1 / 4);

  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  scrollok(*win, TRUE);

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "Process window");
  wattroff(*win, A_BOLD);
  wrefresh(*win);
}

void load_file_name_window(WINDOW **win) {
  char filename[245];

  *win = newwin(getmaxy(stdscr) * 1 / 4, getmaxx(stdscr) / 4,
                getmaxy(stdscr) - (getmaxy(stdscr) * 1 / 4), 0);

  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "File");
  wattroff(*win, A_BOLD);

  mvwprintw(*win, 2, 2, "Ctrl - C to exit");
  mvwprintw(*win, 5, 2, "Insert file name: ");
  echo();
  wgetnstr(*win, filename, sizeof(filename) - 1);
  noecho();

  process_create_syscall(filename);

  wrefresh(*win);
}

void load_memory_window(WINDOW **win) {

  int height = getmaxy(stdscr) - getmaxy(stdscr) / 2;
  int starty = 0 + getmaxy(stdscr) / 2;
  int width = getmaxx(stdscr) * (3 / 4);
  int startx = getmaxx(stdscr) * 1 / 4;

  *win = newwin(height, width, starty, startx);

  wbkgd(*win, COLOR_PAIR(1));
  wattron(*win, COLOR_PAIR(2));

  scrollok(*win, TRUE);

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "Memory window");
  wattroff(*win, A_BOLD);

  wrefresh(*win);
}

char *get_process_filename() {
  int maxy, maxx;
  getmaxyx(stdscr, maxy, maxx);

  /// Calcula as coordenadas para posicionar a janela no centro da tela
  int starty = (maxy - 5) / 2;
  int startx = (maxx - 40) / 2;

  /// Cria a janela para o nome do arquivo
  WINDOW *file_win = newwin(5, 40, starty, startx);
  if (file_win == NULL) {
    return NULL;
  }

  char *filename = (char *)malloc(80 * sizeof(char));
  if (filename == NULL) {
    delwin(file_win);
    return NULL;
  }

  char *input = "File name: ";
  mvwprintw(file_win, 1, 1, "%s", input);
  wbkgd(file_win, COLOR_PAIR(3));
  box(file_win, 0, 0);

  wrefresh(file_win);

  wgetstr(file_win, filename);

  touchwin(stdscr);
  delwin(file_win);
  refresh();
  return filename;
}

void init_log() {
  pthread_t log_id;
  pthread_attr_t log_attr;

  pthread_attr_init(&log_attr);

  pthread_attr_setscope(&log_attr, PTHREAD_SCOPE_SYSTEM);

  pthread_create(&log_id, NULL, (void *)refresh_log, NULL);
}

void refresh_log() {
  while (!LOGS->header)
    ;

  current_log = LOGS->header;
  LogMessage *curr = (LogMessage *)current_log->data;

  int start_processy = 3;
  int start_memoryy = 2;

  while (1) {
    sem_wait(&log_semaphore);
    /// semaphored liberated so there is a new log message

    while (current_log != LOGS->tail) {
      current_log = current_log->next;
      curr = (LogMessage *)current_log->data;
      switch (curr->log_type) {
      case PROCESS_LOG:

        if (start_processy == getmaxy(stdscr) / 2) {
          delwin(process_state_window);
          load_process_window(&process_state_window);
          start_processy = 2;
        }
        mvwprintw(process_state_window, start_processy, 8, curr->log_message);
        wrefresh(process_state_window);
        start_processy++;
        break;

      case MEMORY_LOG:

        if (start_memoryy == getmaxy(stdscr) / 2) {
          delwin(memory_state_window);
          load_memory_window(&memory_state_window);
          start_memoryy = 2;
        }
        mvwprintw(memory_state_window, start_memoryy, 8, curr->log_message);
        wrefresh(memory_state_window);
        start_memoryy++;
        break;

      default:
        exit(1);
      }
    }
  }
}
