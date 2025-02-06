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

void save_scores(const char *filename, Player players[], int n_players) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        mvprintw(1, 1, "Error: Unable to save scores.");
        refresh();
        return;
    }

    for (int i = 0; i < n_players; i++) {
        fprintf(file, "%s %d %d %d %s\n", 
                players[i].username, 
                players[i].total_points, 
                players[i].gold_points, 
                players[i].games_completed, 
                players[i].first_game_time);
    }

    fclose(file);
}
void update_scoreboard(const char *filename) {
    Player players[100];
    int n_players = read_scores(filename, players, 100);
    int found = 0;

    for (int i = 0; i < n_players; i++) {
        if (strcmp(players[i].username, player_username) == 0) {
            players[i].total_points += player.score;
            players[i].gold_points += (player.gold + player.black_gold);
            players[i].games_completed += 1;
            found = 1;
            break;
        }
    }

    if (!found && n_players < 100) {
        Player current_player;
        strcpy(current_player.username, player_username);
        current_player.total_points = player.score;
        current_player.gold_points = (player.gold + player.black_gold);
        current_player.games_completed = 1;
        
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char date_str[11];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
        strcpy(current_player.first_game_time, date_str);

        players[n_players++] = current_player;
    }

    save_scores(filename, players, n_players);
}

void updateMessages(WINDOW *messageWin, const char *message) {
    werase(messageWin);
    wattron(messageWin, COLOR_PAIR(5));
    box(messageWin, 0, 0);
    mvwprintw(messageWin, 1, 1, "%s", message);
    wattroff(messageWin, COLOR_PAIR(5));
    wrefresh(messageWin);
}

// Functions to update player status
void updateStatus(WINDOW *statusWin, PlayerStatus *status) {
    werase(statusWin);
    wattron(statusWin, COLOR_PAIR(5));
    box(statusWin, 0, 0);
    wattroff(statusWin, COLOR_PAIR(5));
    //init_pair(0, COLOR_MAGENTA, COLOR_BLACK);
    wattron(statusWin, COLOR_PAIR(4));
    mvwprintw(statusWin, 1, 1, "Floor: %d", status->floor);
    mvwprintw(statusWin, 1, 15, "Gold: %d", status->gold);
    mvwprintw(statusWin, 1, 30, "Black Gold: %d", status->black_gold);
    mvwprintw(statusWin, 1, 50, "Score: %d", status->score);
    wattroff(statusWin, COLOR_PAIR(4));

    int max_hp = 100;
    int hp_bar_length = 20;
    int filled_blocks = (status->hp * hp_bar_length) / max_hp;

    int color_pair = (status->hp > 50) ? 6 : (status->hp > 20) ? 4 : 3;

    wattron(statusWin, COLOR_PAIR(color_pair));

    int hp_x_pos = COLS - 30;
    mvwprintw(statusWin, 1, hp_x_pos, "HP: [");

    for (int i = 0; i < hp_bar_length; i++) {
        if (i < filled_blocks)
            wprintw(statusWin, "█");
        else
            wprintw(statusWin, "░");
    }

    wprintw(statusWin, "]%d%%", status->hp);
    wattroff(statusWin, COLOR_PAIR(color_pair));

    wrefresh(statusWin);
}


