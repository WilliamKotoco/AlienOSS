#include "main-window.h"

extern List *PCB;
extern List *LOGS;
extern sem_t log_semaphore;

/// global windows
WINDOW *option_window;
/// secondary windows
WINDOW *process_state_window;
WINDOW *memory_state_window;
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
  load_memory_window(&memory_state_window);

  //  load_memory_window(memory_state_window);
  /// get the maximum from the standard screen
  option_window = newwin(getmaxy(stdscr), getmaxx(stdscr) / 4, 0, 0);

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
        char *input = get_process_filename();
        load_process_window(&process_state_window);

        load_memory_window(&memory_state_window);
        wrefresh(process_state_window); /// redrawing updated process window
        wrefresh(memory_state_window);
        display_ascii_art(option_window); /// reloading option window
        box(option_window, 0, 0);
        // rebuild_all_log();

        process_create_syscall(input);

      } else if (highlight == 1) {
        mvwprintw(option_window, 35, 1, "Toggled"); /// temporary

      } else {
        // delwin(stdscr);
        delwin(option_window);
        delwin(process_state_window);
        return;
      }

      break;

    default:
      break;
    }
  }

  wrefresh(option_window);

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

  int start_processy = 2;
  int start_memoryy = 2;
  while (1) {

    int sem_val;
    sem_getvalue(&log_semaphore, &sem_val);
    sem_wait(&log_semaphore);
    /// semaphored liberated so there is a new log message

    while (current_log != LOGS->tail) {
      LogMessage *curr = (LogMessage *)current_log->data;
      switch (curr->log_type) {
      case PROCESS_LOG:
        // mvprintw(process_state_window,start_processy, 5, curr->log_message);
        mvwprintw(process_state_window, start_processy, 8, curr->log_message);
        wrefresh(process_state_window);
        start_processy++;
        break;

      case MEMORY_LOG:
        // mvprintw(memory_state_window,start_memoryy, 5, curr->log_message);
        mvwprintw(memory_state_window, start_memoryy, 8, curr->log_message);

        wrefresh(memory_state_window);
        start_memoryy++;
        break;

      default:
        exit(1);
      }
      current_log = current_log->next;
    }
  }
}

void rebuild_all_log() {

  // Node *node = LOGS->header;
  // int start_processy = 2;
  // int start_memoryy = 2;
  //   while (node != LOGS->tail)
  //   {
  //     LogMessage *curr = (LogMessage *)node->data;
  //     switch(curr->log_type)
  //     {
  //       case PROCESS_LOG:
  //       mvwprintw(process_state_window,start_processy, 5, curr->log_message);
  //       wrefresh(process_state_window);
  //       start_processy++;
  //       break;

  //       case MEMORY_LOG:
  //       mvwprintw(memory_state_window,start_memoryy, 5, curr->log_message);
  //       wrefresh(memory_state_window);
  //       start_memoryy++;
  //       break;

  //       default:
  //       exit(1);

  //     }

  //     free(curr);
  //     node = node->next;
  //   }
}