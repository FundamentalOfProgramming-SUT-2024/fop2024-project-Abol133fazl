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

void generate_treasure_map() {
    initializeMap();
    player.floor = 5;
    roomCount = 0;
    messageWin = newwin(3, MAP_WIDTH + 2, 0, 0);
    mapWin = newwin(MAP_HEIGHT + 2, MAP_WIDTH + 2, 3, 0);
    statusWin = newwin(3, MAP_WIDTH + 2, MAP_HEIGHT + 5, 0);

    refresh();

    Room treasureRoom;
    treasureRoom.width = 80;
    treasureRoom.height = 20;
    treasureRoom.x = (MAP_WIDTH - treasureRoom.width) / 2;
    treasureRoom.y = (MAP_HEIGHT - treasureRoom.height) / 2;
    treasureRoom.enemycount = 0;
    treasureRoom.enemies = (Enemy *)malloc(10 * sizeof(Enemy)); 
    treasureRoom.theme = 8;
    rooms[roomCount++] = treasureRoom;

    //drawRoom(mapWin, treasureRoom);

    for (int i = 0; i < treasureRoom.height; i++) {
        for (int j = 0; j < treasureRoom.width; j++) {
            int x = treasureRoom.x + j;
            int y = treasureRoom.y + i;

            if (i == 0) {
                // roof
                map[player.floor - 1][y][x].type = '_';
                map[player.floor - 1][y][x].display = '_';
                map[player.floor - 1][y][x].color = 8;
            } else if (i == treasureRoom.height - 1) {
                // roof and walls
                if (j == 0 || j == treasureRoom.width - 1) {
                    map[player.floor - 1][y][x].type = '|';
                    map[player.floor - 1][y][x].display = '|';
                    map[player.floor - 1][y][x].color = 8;
                } else {
                    map[player.floor - 1][y][x].type = '_';
                    map[player.floor - 1][y][x].display = '_';
                    map[player.floor - 1][y][x].color = 8;
                }
            } else {
                // wasll and inside treasureRoom
                if (j == 0 || j == treasureRoom.width - 1) {
                    map[player.floor - 1][y][x].type = '|';
                    map[player.floor - 1][y][x].display = '|';
                    map[player.floor - 1][y][x].color = 8;
                } else {
                    map[player.floor - 1][y][x].type = '.';
                    map[player.floor - 1][y][x].display = '.';
                    map[player.floor - 1][y][x].color = 8;
                }
            }
        }
    }

    for (int i = 0; i < 30; i++) {
        int tx = treasureRoom.x + 1 + rand() % (treasureRoom.width - 2);
        int ty = treasureRoom.y + 1 + rand() % (treasureRoom.height - 2);
        map[player.floor - 1][ty][tx].type = '^';
        map[player.floor - 1][ty][tx].display = '^';
        unimap[player.floor - 1][ty][tx] = L'^';
        map[player.floor - 1][ty][tx].color = 9;
    }
    for (int i = 0; i < 30; i++) {
        int tx = treasureRoom.x + 1 + rand() % (treasureRoom.width - 2);
        int ty = treasureRoom.y + 1 + rand() % (treasureRoom.height - 2);
        map[player.floor - 1][ty][tx].type = '*';
        map[player.floor - 1][ty][tx].display = '*';
        unimap[player.floor - 1][ty][tx] = L'★';
        map[player.floor - 1][ty][tx].color = 4;
    }

    int treasure_x = treasureRoom.x + treasureRoom.width / 2;
    int treasure_y = treasureRoom.y + treasureRoom.height / 2;
    map[player.floor - 1][treasure_y][treasure_x].type = '&';
    map[player.floor - 1][treasure_y][treasure_x].display = '&';
    unimap[player.floor - 1][treasure_y][treasure_x] = L'♛';
    map[player.floor - 1][treasure_y][treasure_x].color = 8;

    int i = 0;
    while (i < 10) {
        Enemy enemy = createRandomEnemy(&rooms[0], rooms[0].width, rooms[0].height);
        if (enemy.x < 0 || enemy.x >= MAP_WIDTH || enemy.y < 0 || enemy.y >= MAP_HEIGHT) {
            continue;
        }

        rooms[0].enemies[i] = enemy;
        rooms[0].enemycount++;

        map[player.floor - 1][enemy.y][enemy.x].display = enemy.type;
        map[player.floor - 1][enemy.y][enemy.x].color = 0;

        i++;
    }

    player_x = rooms[0].x + 1;
    player_y = rooms[0].y + 1;
    map[player.floor - 1][player_y][player_x].type = 'X';
    map[player.floor - 1][player_y][player_x].display = 'X';
    map[player.floor - 1][player_y][player_x].discovered = true;

    printMap(mapWin);
    updateMessages(messageWin, "You entered the treasure room!");
}
void generate_battle_map() {
    initializeMap();
    player.floor = 6;
    roomCount = 0;
    messageWin = newwin(3, MAP_WIDTH + 2, 0, 0);
    mapWin = newwin(MAP_HEIGHT + 2, MAP_WIDTH + 2, 3, 0);
    statusWin = newwin(3, MAP_WIDTH + 2, MAP_HEIGHT + 5, 0);

    refresh();

    Room battleroom;
    battleroom.width = 40;
    battleroom.height = 15;
    battleroom.x = (MAP_WIDTH - battleroom.width) / 2;
    battleroom.y = (MAP_HEIGHT - battleroom.height) / 2;
    battleroom.enemycount = 0;
    battleroom.enemies = (Enemy *)malloc(10 * sizeof(Enemy)); 
    rooms[roomCount++] = battleroom;

    //drawRoom(mapWin, battleroom);

    for (int i = 0; i < battleroom.height; i++) {
        for (int j = 0; j < battleroom.width; j++) {
            int x = battleroom.x + j;
            int y = battleroom.y + i;

            if (i == 0) {
                // roof
                map[player.floor - 1][y][x].type = '_';
                map[player.floor - 1][y][x].display = '_';
                map[player.floor - 1][y][x].color = 1;
            } else if (i == battleroom.height - 1) {
                // roof and walls
                if (j == 0 || j == battleroom.width - 1) {
                    map[player.floor - 1][y][x].type = '|';
                    map[player.floor - 1][y][x].display = '|';
                    map[player.floor - 1][y][x].color = 1;
                } else {
                    map[player.floor - 1][y][x].type = '_';
                    map[player.floor - 1][y][x].display = '_';
                    map[player.floor - 1][y][x].color = 1;
                }
            } else {
                // wasll and inside battleroom
                if (j == 0 || j == battleroom.width - 1) {
                    map[player.floor - 1][y][x].type = '|';
                    map[player.floor - 1][y][x].display = '|';
                    map[player.floor - 1][y][x].color = 1;
                } else {
                    map[player.floor - 1][y][x].type = '.';
                    map[player.floor - 1][y][x].display = '.';
                    map[player.floor - 1][y][x].color = 1;
                }
            }
        }
    }

    int battle_x = battleroom.x + battleroom.width / 2;
    int battle_y = battleroom.y + battleroom.height / 2;
    map[player.floor - 1][battle_y][battle_x].type = '&';
    map[player.floor - 1][battle_y][battle_x].display = '&';
    unimap[player.floor - 1][battle_y][battle_x] = L'🚪';
    map[player.floor - 1][battle_y][battle_x].color = 1;

    int i = 0;
    while (i < 5) {
        Enemy enemy = createRandomEnemy(&rooms[0], rooms[0].width, rooms[0].height);
        if (enemy.x < 0 || enemy.x >= MAP_WIDTH || enemy.y < 0 || enemy.y >= MAP_HEIGHT) {
            continue;
        }

        rooms[0].enemies[i] = enemy;
        rooms[0].enemycount++;

        map[player.floor - 1][enemy.y][enemy.x].display = enemy.type;
        map[player.floor - 1][enemy.y][enemy.x].color = 0;

        i++;
    }

    player_x = rooms[0].x + 1;
    player_y = rooms[0].y + 1;
    map[player.floor - 1][player_y][player_x].type = 'X';
    map[player.floor - 1][player_y][player_x].display = 'X';
    map[player.floor - 1][player_y][player_x].discovered = true;

    printMap(mapWin);
    updateMessages(messageWin, "You entered the treasure room!");
}