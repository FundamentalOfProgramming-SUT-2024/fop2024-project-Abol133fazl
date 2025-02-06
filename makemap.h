#ifndef MAKEMAP_H
#define MAKEMAP_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <wchar.h>
#include "menus.h"

// Define constants
#define MAP_WIDTH 154
#define MAP_HEIGHT 36
#define MIN_ROOM_SIZE 10
#define MAX_ROOM_SIZE 15
#define MAX_ROOMS 8
#define MAX_FLOORS 10

// Structures
typedef struct {
    char type;
    int health;
    int damage;
    int x, y;
} Enemy;

typedef struct {
    int x, y;
    int width, height;
    bool haspillar;
    bool visited;
    int enemycount;
    Enemy *enemies;
    int theme;
} Room;

typedef struct {
    int floor;
    int gold;
    int black_gold;
    int hp;
    int hunger;
    int score;
    int difficulty;
    int kills;
} PlayerStatus;

typedef struct {
    char type;
    bool discovered;
    char display;
    int color;
} Cell;

typedef struct {
    int sword;
    int dager;
    int magic_wand;
    int arrow;
} Inventory;



// Extern variables
extern Cell map[MAX_FLOORS][MAP_HEIGHT][MAP_WIDTH];
extern wchar_t unimap[MAX_FLOORS][MAP_HEIGHT][MAP_WIDTH];
extern Room rooms[MAX_ROOMS];
extern PlayerStatus player;
extern Inventory inventory;
extern char *weapons_name[5]; extern int weapons_count[5]; extern int weapons_damage[5]; extern int weapons_length[5]; extern int current_weapon;
extern int foods_count[4]; extern char *foods_name[4];
extern int spells_count[3]; extern char *spells_name[3];
extern int roomCount;
extern int spellroomcount;
extern int player_x, player_y;
extern int stair_x, stair_y;
extern int trap_x, trap_y;
extern int gold_x, gold_y;
extern int food_x, food_y;
extern int spell_x, spell_y;
extern int weapon_x, weapon_y;
extern int auto_dx, auto_dy;
extern bool autoMove;
extern char last_char;
extern wchar_t last_uni;
extern int last_color;
extern int last_theme, cur_theme;
extern WINDOW *messageWin;
extern WINDOW *mapWin;
extern WINDOW *statusWin;
extern bool all_map_discovered;
extern bool g_is_on;
extern bool nightmare_is_on;
extern bool prevlevel;
//extern const char *music_list[4];
extern const int music_count;
extern Mix_Music *current_music;
extern int current_music_index;
extern int speed_is_on, health_is_on, damage_is_on; extern int speedTime, healthTime, damageTime;
extern int change_food_timer;


// Function prototypes
void display_map();
void display_prev_game();
void save_scores(const char *filename, Player players[], int n_players);
void update_scoreboard(const char *filename);
void play_music(int index);
void initializeMap();
void generate_map();
void printMap(WINDOW *win);
void print_uni(WINDOW *win);
void generate_treasure_map();
void generate_battle_map();
void createTreasureRoom(Room *rooms, int roomCount, WINDOW *win);
void createSpellRoom(Room *rooms, int roomCount, WINDOW *win);
void createNightmareRoom(Room *rooms, int roomCount, WINDOW *win);
void updateMessages(WINDOW *messageWin, const char *message);
void updateStatus(WINDOW *statusWin, PlayerStatus *status);
void drawRoom(WINDOW *win, Room room);
void discoverRoom(WINDOW *win, Room *room);
void drawCorridor(WINDOW *win, int x1, int y1, int x2, int y2, Room *rooms, int roomCount);
void drawpillar(WINDOW *win, Room room);
void drawStairs(Room *rooms, int roomCount, WINDOW *win);
void drawTraps(Room *rooms, int roomCount, WINDOW *win);
void makedoor(int x, int y, Room *rooms, int roomCount);
bool doesOverlap(Room *rooms, int roomCount, Room newRoom);
bool isInsideRoom(int x, int y, Room *rooms, int roomCount);
void drawFoods(Room *rooms, int roomCount, WINDOW *win); void displayFoodMenu();
void drawGolds(Room *rooms, int roomCount, WINDOW *win);
void drawWeapons(Room *rooms, int roomCount, WINDOW *win); void displayWeaponsMenu();
void drawSpells(Room *rooms, int roomCount, WINDOW *win); void displaySpellsMenu();
void addHiddenDoors(Room *rooms, int roomCount);
void drawWindows(Room *rooms, int roomCount, WINDOW *win);
void saveGame(char *input); void loadGame(char *input);
void detect_music();
bool canMoveTo(int x, int y);
void movePlayer(int dx, int dy);
void get_move_from_user();
void generate_treasure_map();
Room *find_room(int player_x, int player_y);

#endif // MAKEMAP_H
