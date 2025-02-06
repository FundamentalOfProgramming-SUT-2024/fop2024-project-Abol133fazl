#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <wchar.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include "makemap.h"
#include "enemies.h"
#include "menus.h"


void show_title_screen() {
    clear();

    char *rogue_art[] = {
    "██████╗░░██████╗░███████╗██╗░░░██╗███████╗",
    "██╔══██╗██╔════╝░██╔════╝██║░░░██║██╔════╝",
    "██████╔╝██║░░██╗░█████╗░░██║░░░██║█████╗░░",
    "██╔══██╗██║░░╚██╗██╔══╝░░██║░░░██║██╔══╝░░",
    "██║░░██║╚██████╔╝███████╗╚██████╔╝███████╗",
    "╚═╝░░╚═╝░╚═════╝░╚══════╝░╚═════╝░╚══════╝",
    "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄",
    "█░▒▓▓████████████████████████████████▓▓▒░█",
    "█░░░▒▒▓▓▓▀▀▀░░░░░░░░░░░░░░░░░░▀▀▀▓▓▓▒▒░░░█",
    "█▒░░░░▒▒▓▓▄░░░░░░░⚔️░░░░░░░▄▓▓▒▒░░░░▒█",
    "██▄▄░░░░░▒▒▓▓▄▄░░░░░░░░▄▄▓▓▒▒░░░░░▄▄██",
    "█████▄▄▄░░░▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒░░░▄▄▄████",
    "████████▀▀▀▀░░░░░░░░░░░░░░░▀▀▀▀███████",
    "░▀▀██████▓▓▓▓▒▒▒░░░░░▒▒▒▓▓▓▓██████▀▀░",
    "░░░░▀▀▀████████████████████████▀▀▀░░░░",
    "░░░░░░░░░░▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀░░░░░░░░░░░"
};
    
    // تنظیم رنگ قرمز برای لوگو
    attron(COLOR_PAIR(3) | A_BOLD);
    
    // نمایش هر خط از اسکی آرت
    for(int i=0; i<sizeof(rogue_art)/sizeof(char*); i++) {
        mvprintw((LINES - 20) / 2 + i, (COLS - 40) / 2, "%s", rogue_art[i]);
    }
    
    // نمایش پیام پایین صفحه
    attroff(COLOR_PAIR(1));
    mvprintw(LINES - 3, (COLS - 20)/2, "Press any key to continue...");
    
    refresh();
    getch();
    clear();
    refresh();
}


void show_lose_message() {
    initscr();
    noecho();
    curs_set(0);

    char *lose[] = {
        "__   __  _______  __   __    ___      _______  _______  _______ ",
        "|  | |  ||       ||  | |  |  |   |    |       ||       ||       |",
        "|  |_|  ||   _   ||  | |  |  |   |    |   _   ||  _____||_     _|",
        "|       ||  | |  ||  |_|  |  |   |    |  | |  || |_____   |   |  ",
        "|_     _||  |_|  ||       |  |   |___ |  |_|  ||_____  |  |   |  ",
        "  |   |  |       ||       |  |       ||       | _____| |  |   |  ",
        "  |___|  |_______||_______|  |_______||_______||_______|  |___|"
    };

    int start_y = (LINES - 7) / 2;
    int start_x = (COLS - 70) / 2;

    for (int i = 0; i < 7; i++) {
        mvprintw(start_y + i, start_x, "%s", lose[i]);
    }

    refresh();
    endwin();
}

void show_win_message() {
    initscr();
    noecho();
    curs_set(0);

    char *lose[] = {
        " __   __  _______  __   __    _     _  _______  __    _", 
        "|  | |  ||       ||  | |  |  | | _ | ||       ||  |  | |",
        "|  |_|  ||   _   ||  | |  |  | || || ||   _   ||   |_| |",
        "|       ||  | |  ||  |_|  |  |       ||  | |  ||       |",
        "|_     _||  |_|  ||       |  |       ||  |_|  ||  _    |",
        "  |   |  |       ||       |  |   _   ||       || | |   |",
        "  |___|  |_______||_______|  |__| |__||_______||_|  |__|"
    };

    int start_y = (LINES - 7) / 2;
    int start_x = (COLS - 70) / 2;

    for (int i = 0; i < 7; i++) {
        mvprintw(start_y + i, start_x, "%s", lose[i]);
    }

    refresh();
    endwin();
}