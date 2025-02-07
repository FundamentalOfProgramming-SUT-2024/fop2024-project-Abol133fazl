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



Cell map[MAX_FLOORS][MAP_HEIGHT][MAP_WIDTH];
wchar_t unimap[MAX_FLOORS][MAP_HEIGHT][MAP_WIDTH];
Room rooms[MAX_ROOMS];
PlayerStatus player = {1, 0, 0, 100, 0, 0, 0, 0};
Inventory inventory = {0, 0, 0, 0};
char *weapons_name[5] = {"Sword", "Dager", "Magic Wand", "Normal Arrow", "Mace"}; int weapons_count[5] = {0, 0, 0, 0, 1}; int weapons_damage[5] = {10, 12, 15, 5, 5}; int weapons_length[5] = {1, 5, 10, 5, 1}; int current_weapon = 4;
int foods_count[4] = {0, 0, 0}; char *foods_name[4] = {"Normal", "Super", "Magic", "Corrupted"};
int spells_count[3] = {0, 0, 0}; char *spells_name[3] = {"Health", "Speed", "Damage"};
int roomCount = 0;
int spellroomcount = 0;
int player_x = 0, player_y = 0;
int stair_x = -1, stair_y = -1;
int trap_x = -1, trap_y = -1;
int gold_x = -1, gold_y = -1;
int food_x = -1, food_y = -1;
int spell_x = -1, spell_y = -1;
int weapon_x = -1, weapon_y = -1;
int auto_dx = 0, auto_dy = 0;
bool autoMove = false;
char last_char = '.';
wchar_t last_uni = L'.';
int last_color;
int last_theme = 1, cur_theme = 1;
WINDOW *messageWin;
WINDOW *mapWin;
WINDOW *statusWin;
bool all_map_discovered = false;
bool g_is_on = false;
bool nightmare_is_on = false;
bool prevlevel = false;
int speed_is_on = 0, health_is_on = 0, damage_is_on = 0; int speedTime = 0, healthTime = 0, damageTime = 0;
int change_food_timer = 20;


// Display whole map
void display_map() {
    static int sdl_initialized = 0;
    if (!sdl_initialized) {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
            printf("Failed to initialize SDL Mixer: %s\n", Mix_GetError());
            return;
        }
        sdl_initialized = 1;
    }

    refresh();
    generate_map();
    
    if (Mix_PlayingMusic() == 0) {
        play_music(default_music - 1);
    }

    refresh();
    get_move_from_user();
    saveGame("");

    delwin(messageWin);
    delwin(mapWin);
    delwin(statusWin);
    
    Mix_HaltMusic();
    Mix_FreeMusic(current_music);
    current_music = NULL;

    endwin();
    clear();
    refresh();
}

void display_prev_game() {
    static int sdl_initialized = 0;
    if (!sdl_initialized) {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
            printf("Failed to initialize SDL Mixer: %s\n", Mix_GetError());
            return;
        }
        sdl_initialized = 1;
    }

    refresh();
    generate_map();
    loadGame("");
    
    if (Mix_PlayingMusic() == 0) {
        play_music(default_music - 1);
    }

    refresh();
    get_move_from_user();

    delwin(messageWin);
    delwin(mapWin);
    delwin(statusWin);
    
    Mix_HaltMusic();
    Mix_FreeMusic(current_music);
    current_music = NULL;

    endwin();
    clear();
    refresh();
}

// Function to initialize map
void initializeMap() {
    for (int f = 0; f < MAX_FLOORS; f++) {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                map[f][i][j].type = ' ';
                map[f][i][j].display = ' ';
                unimap[f][i][j] = L' ';
                map[f][i][j].discovered = false;
            }
        }
    }
}

// Function to print the map
void printMap(WINDOW *win) {
    werase(win);
    box(win, 0, 0);

    
    if (!all_map_discovered) {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                if (map[player.floor - 1][i][j].discovered) {
                    wattron(win, COLOR_PAIR(map[player.floor - 1][i][j].color));
                    mvwaddch(win, i + 1, j + 1, map[player.floor - 1][i][j].display);
                    if (unimap[player.floor - 1][i][j] != L' ') mvwprintw(win, i + 1, j + 1, "%lc ", unimap[player.floor - 1][i][j]);
                    wattroff(win, COLOR_PAIR(map[player.floor - 1][i][j].color));
                } else {
                    mvwaddch(win, i + 1, j + 1, ' ');
                }
            }
        }
    } else {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                char cellol = map[player.floor - 1][i][j].display;
                if (cellol != ' ') {
                    if (map[player.floor - 1][i][j].type == '?') {
                        wattron(win, COLOR_PAIR(map[player.floor - 1][i][j].color));
                        mvwaddch(win, i + 1, j + 1, map[player.floor - 1][i][j].type);
                        if (unimap[player.floor - 1][i][j] != L' ') mvwprintw(win, i + 1, j + 1, "%lc ", unimap[player.floor - 1][i][j]);
                        wattroff(win, COLOR_PAIR(map[player.floor - 1][i][j].color));
                    } else {
                        wattron(win, COLOR_PAIR(map[player.floor - 1][i][j].color));
                        mvwaddch(win, i + 1, j + 1, map[player.floor - 1][i][j].display);
                        if (unimap[player.floor - 1][i][j] != L' ') mvwprintw(win, i + 1, j + 1, "%lc ", unimap[player.floor - 1][i][j]);
                        wattroff(win, COLOR_PAIR(map[player.floor - 1][i][j].color));
                    }
                } else {
                    mvwaddch(win, i + 1, j + 1, ' ');
                }
            }
        }
    }

    wattron(win, COLOR_PAIR(map[player.floor - 1][player_y][player_x].color) | A_BOLD);
    mvwaddch(win, player_y + 1, player_x + 1, 'X');
    wattroff(win, COLOR_PAIR(map[player.floor - 1][player_y][player_x].color) | A_BOLD);

    refresh();
    wrefresh(win);
}

// Function to find the room we are in and its theme
Room *find_room(int player_x, int player_y) {
    if (map[player.floor - 1][player_y][player_x].type == '#') {
        return NULL;
    }
    for (int i = 0; i < roomCount; i++) {
        Room *room = &rooms[i];
        if (player_x >= room->x && player_x < room->x + room->width &&
            player_y >= room->y && player_y < room->y + room->height) {
            return room;
        }
    }
    return NULL;
}

// checking overlap rooms
bool doesOverlap(Room *rooms, int roomCount, Room newRoom) {
    for (int i = 0; i < roomCount; i++) {
        Room r = rooms[i];
        
        //check overlap
        if (newRoom.x < r.x + r.width &&
            newRoom.x + newRoom.width > r.x &&
            newRoom.y < r.y + r.height &&
            newRoom.y + newRoom.height > r.y) {
            return true;
        }

        // check the distance beetwin rooms be at least 10
        int horizontalDist = (newRoom.x > r.x + r.width) ? newRoom.x - (r.x + r.width)
                          : (r.x > newRoom.x + newRoom.width) ? r.x - (newRoom.x + newRoom.width)
                          : 0;

        int verticalDist = (newRoom.y > r.y + r.height) ? newRoom.y - (r.y + r.height)
                        : (r.y > newRoom.y + newRoom.height) ? r.y - (newRoom.y + newRoom.height)
                        : 0;

        if (horizontalDist < 10 && verticalDist < 10) {
            return true; // distance is less than 10
        }
    }
    return false;
}

// check if the cell is inside a room or not
bool isInsideRoom(int x, int y, Room *rooms, int roomCount) {
    for (int i = 0; i < roomCount; i++) {
        Room r = rooms[i];
        if (x >= r.x && x < r.x + r.width && y >= r.y && y < r.y + r.height) {
            return true;
        }
    }
    return false;
}

//Function to draw a room
void drawRoom(WINDOW *win, Room room) {
    for (int i = 0; i < room.height; i++) {
        for (int j = 0; j < room.width; j++) {
            int x = room.x + j;
            int y = room.y + i;

            if (i == 0) {
                // roof
                map[player.floor - 1][y][x].type = '_';
                map[player.floor - 1][y][x].display = '_';
                map[player.floor - 1][y][x].color = 1;
            } else if (i == room.height - 1) {
                // roof and walls
                if (j == 0 || j == room.width - 1) {
                    map[player.floor - 1][y][x].type = '|';
                    map[player.floor - 1][y][x].display = '|';
                    map[player.floor - 1][y][x].color = 1;

                } else {
                    map[player.floor - 1][y][x].type = '_';
                    map[player.floor - 1][y][x].display = '_';
                    map[player.floor - 1][y][x].color = 1;

                }
            } else {
                // wasll and inside room
                if (j == 0 || j == room.width - 1) {
                    map[player.floor - 1][y][x].type = '|';
                    map[player.floor - 1][y][x].display = '|';
                    map[player.floor - 1][y][x].color = 1;

                } else {
                    map[player.floor - 1][y][x].type = '.';
                    map[player.floor - 1][y][x].display = '.';
                    map[player.floor - 1][y][x].color = 0;
                }
            }

            map[player.floor - 1][y][x].discovered = false;
        }
    }
    drawpillar(win, room);
}

// Function to discover a room
void discoverRoom(WINDOW *win, Room *room) {
    if (!room->visited) {
        room->visited = true;
        for (int i = 0; i < room->height; i++) {
            for (int j = 0; j < room->width; j++) {
                int x = room->x + j;
                int y = room->y + i;
                map[player.floor - 1][y][x].discovered = true;
            }
        }
        printMap(win);
    }
}

// Function to generate the whole map
void generate_map() {
    initializeMap();

    roomCount = 0;
    messageWin = newwin(3, MAP_WIDTH + 2, 0, 0);
    mapWin = newwin(MAP_HEIGHT + 2, MAP_WIDTH + 2, 3, 0);
    statusWin = newwin(3, MAP_WIDTH + 2, MAP_HEIGHT + 5, 0);

    refresh();
    int attempts = 0;
    while (roomCount <= MAX_ROOMS && attempts < 1000000) {
        Room newRoom;
        newRoom.width = rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1) + MIN_ROOM_SIZE;
        newRoom.height = rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1) + MIN_ROOM_SIZE;
        newRoom.x = rand() % (MAP_WIDTH - newRoom.width - 1) + 1;
        newRoom.y = rand() % (MAP_HEIGHT - newRoom.height - 1) + 1;
        newRoom.haspillar = ((rand() % 2 + 1) == 2) ? 1 : 0;
        newRoom.enemycount = 0;
        newRoom.enemies = (Enemy *)malloc(3 * sizeof(Enemy));

        if (!doesOverlap(rooms, roomCount, newRoom)) {
            rooms[roomCount] = newRoom;
            roomCount++;
        }
        attempts++;
    }
    

    refresh();

    box(messageWin, 0, 0);
    box(mapWin, 0, 0);
    box(statusWin, 0, 0);

    wrefresh(messageWin);
    wrefresh(mapWin);
    wrefresh(statusWin);

    updateMessages(messageWin, "Welcome to the Rogue!");
    updateStatus(statusWin, &player);

    // Creating rooms
    for (int i = 0; i < roomCount; i++) {
        drawRoom(mapWin, rooms[i]);
        rooms[i].theme = 1;

        if (i > 0) { // draw corridors beetwin last and next rooms
            int prevX = rooms[i - 1].x + rooms[i - 1].width / 2;
            int prevY = rooms[i - 1].y + rooms[i - 1].height / 2;
            int currX = rooms[i].x + rooms[i].width / 2;
            int currY = rooms[i].y + rooms[i].height / 2;
            drawCorridor(mapWin, prevX, prevY, currX, currY, rooms, roomCount);
        }
    }

    refresh();
    for (int i = 0; i < MAP_WIDTH; i++) {
        for (int j = 0; j < MAP_HEIGHT; j++) {
            makedoor(i, j, rooms, roomCount);
        }
    }
    
    drawWindows(rooms, roomCount, mapWin);
    drawTraps(rooms, roomCount, mapWin);
    drawGolds(rooms, roomCount, mapWin);
    drawWeapons(rooms, roomCount, mapWin);
    drawFoods(rooms, roomCount, mapWin);
    drawSpells(rooms, roomCount, mapWin);
    createTreasureRoom(rooms, roomCount, mapWin);
    createSpellRoom(rooms, roomCount, mapWin);
    createNightmareRoom(rooms, roomCount, mapWin);
    addHiddenDoors(rooms, roomCount);
    populateEnemies(rooms, roomCount);
    drawStairs(rooms, roomCount, mapWin);
    wrefresh(mapWin);

    int i = 0;
    while (i < roomCount) {
        Room *stairroom = find_room(stair_x, stair_y);
        player_x = rooms[i].x + rooms[i].width / 2;
        player_y = rooms[i].y + rooms[i].height / 2;
        Room *temp = find_room(player_x, player_y);
        if (temp->theme == 1 && temp != stairroom) break;
        i++;
    }
    map[player.floor - 1][player_y][player_x].type = 'X';
    map[player.floor - 1][player_y][player_x].display = 'X';
    map[player.floor - 1][player_y][player_x].discovered = true;
    //prevmap = &map[player.floor - 1];
    printMap(mapWin);
}

