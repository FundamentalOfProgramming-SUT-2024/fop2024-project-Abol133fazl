#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <wchar.h>
#include "makemap.h"
#include "enemies.h"
#include "menus.h"

int main() {
    setlocale(LC_ALL, "");

    srand(time(0));
    initscr();
    noecho();
    curs_set(0);
    raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK); // player character
    init_pair(1, COLOR_BLUE, COLOR_BLACK);  // normal rooms
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK); // corridors
    init_pair(3, COLOR_RED, COLOR_BLACK); //stairs & traps
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // gold
    init_pair(5, COLOR_CYAN, COLOR_BLACK); // weapeon
    init_pair(6, COLOR_GREEN, COLOR_BLACK); // food
    init_pair(7, COLOR_BLACK, COLOR_WHITE); // black gold
    init_pair(8, COLOR_YELLOW, COLOR_BLACK); /* treasureroom */ init_pair(9, COLOR_RED, COLOR_BLACK); /*treasure room trap*/
    init_color(10, 1000, 537, 737); init_pair(10, 10, COLOR_BLACK); // spell room
    init_color(11, 0, 200, 200); init_pair(11, 11, COLOR_BLACK); // nightmare room
    init_pair(12, COLOR_WHITE, COLOR_RED); // hit effect

    show_title_screen();

    char *main_menu[] = { "Create New User", "Enter User", "Exit" }; // list of menus in main_menu
    int n_main_menu = sizeof(main_menu) / sizeof(char *);
    int highlight = 0;

    
    int width = 40;
    int height = 10;
    int startx = (COLS - width) / 2;
    int starty = (LINES - height) / 2;
    WINDOW *menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);

    
    while(1) {
        display_menu(menu_win, highlight, main_menu, n_main_menu);
        int ch = wgetch(menu_win);

        
        if (ch == KEY_UP) {
            if (highlight == 0) {
                highlight = n_main_menu - 1;
            } else {
                highlight--;
            }
        } else if (ch == KEY_DOWN) {
            if (highlight == n_main_menu - 1) {
                highlight = 0;
            } else {
                highlight++;
            }
        } else if (ch == 10) {
            if (highlight == 0) {
                create_new_user();
            } else if (highlight == 1) {
                enter_user();
            }else if (highlight == 2) {
                break;
            }
        } else if (ch == 'q')  {
            break;
        }
    }

    endwin();

    return 0;
}