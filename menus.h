#ifndef MENUS_H
#define MENUS_H

#include <ncursesw/ncurses.h>

#define MAX_PLAYERS 100
#define PLAYERS_PER_PAGE 10

typedef struct {
    char username[50];
    int total_points;
    int gold_points;
    int games_completed;
    char first_game_time[20];
} Player;

extern int game_difficulty;
extern int character_color;
extern char *player_username;
extern bool music_enabled;
extern int default_music;

void display_menu(WINDOW *menu_win, int highlight, char *main_menu[], int n_main_menu);
void create_new_user();
void generate_random_password(char *password, int length);
void enter_user();
void pre_game_menu();
void display_scoreboard();
int read_scores(const char *filename, Player players[], int max_players);
void display_options();
void profile_menu();
void show_title_screen();

#endif
