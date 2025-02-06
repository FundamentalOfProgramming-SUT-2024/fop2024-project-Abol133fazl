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

// Function to create themes
void createTreasureRoom(Room *rooms, int roomCount, WINDOW *win) {
    if (player.floor != 4) return;

    int roomIndex = rand() % roomCount;
    Room treasureRoom = rooms[roomIndex];
    rooms[roomIndex].theme = 8;

    for (int i = 0; i < treasureRoom.height; i++) {
        for (int j = 0; j < treasureRoom.width; j++) {
            int x = treasureRoom.x + j;
            int y = treasureRoom.y + i;

            if (i == 0 || i == treasureRoom.height - 1 || j == 0 || j == treasureRoom.width - 1) {
                map[player.floor - 1][y][x].color = 8;
            } else {
                map[player.floor - 1][y][x].type = '.';
                map[player.floor - 1][y][x].display = '.';
                unimap[player.floor - 1][y][x] = L'.';
                map[player.floor - 1][y][x].color = 0;
            }
        }
    }

    int numTraps = (treasureRoom.width * treasureRoom.height) / 5; // 20 percent of room surface
    for (int t = 0; t < numTraps; t++) {
        int trap_x = treasureRoom.x + 1 + rand() % (treasureRoom.width - 2);
        int trap_y = treasureRoom.y + 1 + rand() % (treasureRoom.height - 2);

        map[player.floor - 1][trap_y][trap_x].type = '^';
        map[player.floor - 1][trap_y][trap_x].display = '^';
        unimap[player.floor - 1][trap_y][trap_x] = L'^';
        map[player.floor - 1][trap_y][trap_x].color = 9;
    }

    // put treasure
    int attempts = 0;
    int treasure_x = treasureRoom.x/2 + 1, treasure_y = treasureRoom.y/2 + 1;
    while (attempts < 100000) {
        treasure_x = treasureRoom.x + 1 + rand() % (treasureRoom.width - 2);
        treasure_y = treasureRoom.y + 1 + rand() % (treasureRoom.height - 2);
        attempts++;
        if (map[player.floor - 1][treasure_y][treasure_x + 1].type == '.' && map[player.floor - 1][treasure_y][treasure_x - 1].type == '.') break;
    }

    map[player.floor - 1][treasure_y][treasure_x].type = '8';
    map[player.floor - 1][treasure_y][treasure_x].display = '8';
    unimap[player.floor - 1][treasure_y][treasure_x] = L'♛';
    map[player.floor - 1][treasure_y][treasure_x].color = 8;
}
void createSpellRoom(Room *rooms, int roomCount, WINDOW *win) {
    if (player.floor == 4) return;
    int roomIndex = rand() % roomCount;
    Room spellRoom = rooms[roomIndex];
    rooms[roomIndex].theme = 10;

    for (int i = 0; i < spellRoom.height; i++) {
        for (int j = 0; j < spellRoom.width; j++) {
            int x = spellRoom.x + j;
            int y = spellRoom.y + i;

            if (i == 0 || i == spellRoom.height - 1 || j == 0 || j == spellRoom.width - 1) {
                map[player.floor - 1][y][x].color = 10;
                if (map[player.floor - 1][y][x].type == '+') {
                    map[player.floor - 1][y][x].type = '?';
                    map[player.floor - 1][y][x].display = (i == 0 || (i == spellRoom.height - 1 && j != 0)) ?'_' :'|';
                }
            } else {
                map[player.floor - 1][y][x].color = 0;
                map[player.floor - 1][y][x].type = '.';
                map[player.floor - 1][y][x].display = '.';
                unimap[player.floor - 1][y][x] = L'.';
            }
        }
    }

    int numSpells = (spellRoom.width * spellRoom.height) / 5; // 20 percent of room surface
    for (int t = 0; t < numSpells; t++) {
        int spell_x = spellRoom.x + 1 + rand() % (spellRoom.width - 2);
        int spell_y = spellRoom.y + 1 + rand() % (spellRoom.height - 2);

        map[player.floor - 1][spell_y][spell_x].type = '%';
        map[player.floor - 1][spell_y][spell_x].display = '%';
        unimap[player.floor - 1][spell_y][spell_x] = L'▲';
        int type = rand() % 3;
        if (type == 0) {
            map[player.floor - 1][spell_y][spell_x].color = 10;
        } else if (type == 1) {
            map[player.floor - 1][spell_y][spell_x].color = 1;
        } else if (type == 2) {
            map[player.floor - 1][spell_y][spell_x].color = 5;
        }
    }
}
void createNightmareRoom(Room *rooms, int roomCount, WINDOW *win) {
    int roomIndex;
    Room nightmareRoom;
    int check_type;
    if (player.floor == 4 || player.floor == 1) return;
    do {
        roomIndex = rand() % roomCount;
        nightmareRoom = rooms[roomIndex];
        check_type = map[player.floor - 1][nightmareRoom.y][nightmareRoom.x].color;
    } while (check_type == 10);
    rooms[roomIndex].theme = 11;

    for (int i = 0; i < nightmareRoom.height; i++) {
        for (int j = 0; j < nightmareRoom.width; j++) {
            int x = nightmareRoom.x + j;
            int y = nightmareRoom.y + i;

            if (i == 0 || i == nightmareRoom.height - 1 || j == 0 || j == nightmareRoom.width - 1) {
                map[player.floor - 1][y][x].color = 11;
            } else {
                map[player.floor - 1][y][x].type = '.';
                map[player.floor - 1][y][x].color = 0;
            }
        }
    }
}