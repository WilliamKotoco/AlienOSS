#include "main-window.h"

extern List *PCB;
void show_and_run() {

  WINDOW *option_window;
  /// secondary windows
  WINDOW *process_state_window;
  WINDOW *memory_state_window;

  initscr();
  cbreak();

  start_color();
  init_pair(1, COLOR_BLACK, COLOR_BLACK); /// background color
  init_pair(2, COLOR_GREEN, COLOR_BLACK); /// Alien green color
  init_pair(3, COLOR_GREEN, COLOR_WHITE); /// Color for inputting from user
  bkgd(COLOR_PAIR(1));

  load_process_window(&process_state_window);

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
        mvwprintw(process_state_window, 10, 5, input);

        wrefresh(process_state_window);   /// redrawing updated process window
        display_ascii_art(option_window); /// reloading option window
        box(option_window, 0, 0);

        // process_create_syscall(input);

        /// TEMP
        // Process *teste = (Process *)PCB->header->data;
        // mvwprintw(process_state_window, 11, 5,
        //  "Cabeça da lista de processos: ID %d Name %s\n", teste->id,
        //  teste->name);
        // wrefresh(process_state_window);
        // display_ascii_art(option_window); /// reloading option window
        // box(option_window, 0, 0);
        process_create_syscall(input);

      } else if (highlight == 1) {
        mvwprintw(option_window, 35, 1, "Toggled"); /// temporary

      } else {
        //delwin(stdscr);
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

  box(*win, 0, 0);

  wattron(*win, A_BOLD);
  mvwprintw(*win, 0, 2, "Process window");
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
