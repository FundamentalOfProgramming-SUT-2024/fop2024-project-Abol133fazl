#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <wchar.h>
#include "makemap.h"
#include "enemies.h"

// Functions to create enemy
Enemy createRandomEnemy(Room *room, int roomWidth, int roomHeight) {
    Enemy enemy;

    int enemyType = rand() % 5;
    switch (enemyType) {
        case 0: enemy.type = 'D'; enemy.health = 5; enemy.damage = 1; break; // Deamon
        case 1: enemy.type = 'F'; enemy.health = 10; enemy.damage = 2; break; // Fire Monster
        case 2: enemy.type = 'G'; enemy.health = 15; enemy.damage = 3; break; // Giant
        case 3: enemy.type = 'S'; enemy.health = 20; enemy.damage = 5; break; // Snake
        case 4: enemy.type = 'U'; enemy.health = 30; enemy.damage = 10; break; // Undead
    }

    if (roomWidth < 3 || roomHeight < 3) {
        printf("Error: Room size too small for enemy placement.\n");
        enemy.x = -1;
        enemy.y = -1;
        return enemy;
    }

    do {
        enemy.x = room->x + 1 + rand() % (roomWidth - 2);
        enemy.y = room->y + 1 + rand() % (roomHeight - 2);
    } while (enemy.x < 0 || enemy.x >= MAP_WIDTH || enemy.y < 0 || enemy.y >= MAP_HEIGHT ||
             map[player.floor - 1][enemy.y][enemy.x].type != '.');

    return enemy;
}
int addEnemyToRoom(Room *room, Enemy enemy) {
    if (enemy.x < 0 || enemy.x >= MAP_WIDTH || enemy.y < 0 || enemy.y >= MAP_HEIGHT) {
        printf("Error: Invalid enemy position (%d, %d).\n", enemy.x, enemy.y);
        return 0;
    }

    if (room->enemycount >= 1) {
        return 0;
    }

    room->enemies[0] = enemy;
    room->enemycount++;

    map[player.floor - 1][enemy.y][enemy.x].display = enemy.type;
    //map[player.floor - 1][enemy.y][enemy.x].type = (enemy.type == 'D') ? enemy.type : (enemy.type == 'F') ? enemy.type : '.';
    map[player.floor - 1][enemy.y][enemy.x].color = 0;

    return 1;
}
void populateEnemies(Room *rooms, int roomCount) {
    int enemiesToAdd = rand() % 5 + 1;

    for (int i = 0; i < enemiesToAdd; i++) {
        int roomIndex = rand() % roomCount;

        if (rooms[roomIndex].enemycount == 0) {
            Enemy enemy = createRandomEnemy(&rooms[roomIndex], rooms[roomIndex].width, rooms[roomIndex].height);

            if (enemy.x != -1 && enemy.y != -1) {
                addEnemyToRoom(&rooms[roomIndex], enemy);
            }
        }
    }
}

// Functions to move enemy
void moveEnemyTowardsPlayer(Enemy *enemy) {
    int dx = 0, dy = 0;
    if (rand() % 2) {
        if (enemy->x < player_x) dx = 1;
        else if (enemy->x > player_x) dx = -1;
    } else {
        if (enemy->y < player_y) dy = 1;
        else if (enemy->y > player_y) dy = -1;
    }

    int new_x = enemy->x + dx;
    int new_y = enemy->y + dy;
    int last_color = map[player.floor - 1][new_y][new_x].color;

    if (canMoveTo(new_x, new_y)) {
        
        map[player.floor - 1][enemy->y][enemy->x].display = map[player.floor - 1][enemy->y][enemy->x].type;
        map[player.floor - 1][enemy->y][enemy->x].color = last_color;

        enemy->x = new_x;
        enemy->y = new_y;

        map[player.floor - 1][enemy->y][enemy->x].display = enemy->type;
        map[player.floor - 1][enemy->y][enemy->x].color = 0;
    }
    if (enemy->x == player_x && enemy->y == player_y) {
        player.hp -= enemy->damage;
        if (player.hp < 0) player.hp = 0;
        updateStatus(statusWin, &player);
        updateMessages(messageWin, "You were attacked by a enemy!");
    }
}
void handleEnemiesInRoom(Room *room) {
    for (int i = 0; i < room->enemycount; i++) {
        Enemy *enemy = &room->enemies[i];
        if (enemy->type == 'D' || enemy->type == 'F') {
            if (enemy->x == player_x && enemy->y == player_y) {
                map[player.floor - 1][player_y][player_x].display = enemy->type;
                player.hp -= enemy->damage;
                updateStatus(statusWin, &player);
                updateMessages(messageWin, "You were attacked by a enemy!");
            }
        } else if (enemy->type == 'G' || enemy->type == 'U') {
            
            int distance = abs(enemy->x - player_x) + abs(enemy->y - player_y);
            if (distance <= 3) {
                moveEnemyTowardsPlayer(enemy);
            }
        } else if (enemy->type == 'S') {

            moveEnemyTowardsPlayer(enemy);
        }
    }
}

// Function to hit near enemies
void hitNearEnemies(Room *room, int x, int y) {
    if (room->enemycount == 0) return;
    for (int k = 0; k < room->enemycount; k++) {
        Enemy *enemy = &room->enemies[k];
        if (enemy-> health) {
            for (int i = y - 1; i <= y + 1; i++) {
                for (int  j = x - 1; j <= x + 1; j++) {
                    if (i == enemy->y && j == enemy->x) {
                        enemy->health -= (weapons_damage[current_weapon] + damage_is_on);
                        if (enemy->health <= 0) {
                            enemy->health = 0;
                            map[player.floor - 1][enemy->y][enemy->x].display = '.';
                            map[player.floor - 1][enemy->y][enemy->x].type = '.';
                            updateMessages(messageWin, "You killed the enemy!");
                            player.kills += 1;
                            updateStatus(statusWin, &player);

                            map[player.floor - 1][enemy->y][enemy->x].color = 12;
                            printMap(mapWin);
                            usleep(100000);
                            map[player.floor - 1][enemy->y][enemy->x].color = 0;
                            printMap(mapWin);

                            for (int j = k; j < room->enemycount - 1; j++) {
                                room->enemies[j] = room->enemies[j + 1];
                            }
                            k--;
                            room->enemycount -= 1;
                            return;
                        }
                        map[player.floor - 1][enemy->y][enemy->x].color = 12;
                        printMap(mapWin);
                        usleep(100000);
                        map[player.floor - 1][enemy->y][enemy->x].color = 0;
                        printMap(mapWin);
                        //return;
                    }
                }
            }
        } 
    }
}
void hitFarEnemies(Room *room, int x, int y) {
    if (room->enemycount == 0) return;
    if (weapons_count[current_weapon] <= 0) {
        updateMessages(messageWin, "Not enough amount of weapon");
        return;
    }

    flushinp();
    updateMessages(messageWin, "Enter a direction to attack"); 
    wrefresh(messageWin);

    int dest = getch();
    int maxDistance = weapons_length[current_weapon];

    int newX = x, newY = y;
    int enemyHit = 0;

    for (int i = 0; i < maxDistance; i++) {
        switch (dest) {
            case '6': newX++; break;
            case '4': newX--; break;
            case '8': newY--; break;
            case '2': newY++; break;
            default: return;
        }

        if (newX < 0 || newX >= MAP_WIDTH || newY < 0 || newY >= MAP_HEIGHT) break;

        if (canMoveTo(newX, newY)) {
            int lastcolor = map[player.floor - 1][newY][newX].color;
            map[player.floor - 1][newY][newX].color = 12;
            printMap(mapWin);
            usleep(100000);
            map[player.floor - 1][newY][newX].color = lastcolor;
            printMap(mapWin);
        }

        for (int k = 0; k < room->enemycount; k++) {
            Enemy *enemy = &room->enemies[k];
            if (enemy->health > 0 && enemy->x == newX && enemy->y == newY) {
                enemy->health -= (weapons_damage[current_weapon] + damage_is_on);
                weapons_count[current_weapon] -= 1;
                enemyHit = 1;

                if (enemy->health <= 0) {
                    enemy->health = 0;
                    map[player.floor - 1][enemy->y][enemy->x].display = '.';
                    map[player.floor - 1][enemy->y][enemy->x].type = '.';
                    updateMessages(messageWin, "You killed the enemy!");
                    player.kills += 1;
                    updateStatus(statusWin, &player);

                    map[player.floor - 1][enemy->y][enemy->x].color = 12;
                    printMap(mapWin);
                    usleep(100000);
                    map[player.floor - 1][enemy->y][enemy->x].color = 0;
                    printMap(mapWin);

                    for (int j = k; j < room->enemycount - 1; j++) {
                        room->enemies[j] = room->enemies[j + 1];
                    }
                    room->enemycount--;
                    k--;

                } else {
                    map[player.floor - 1][enemy->y][enemy->x].color = 12;
                    printMap(mapWin);
                    usleep(100000);
                    map[player.floor - 1][enemy->y][enemy->x].color = 0;
                    printMap(mapWin);
                }
            }
        }

        if (enemyHit) break;
    }
    if ((map[player.floor - 1][newY][newX].type == '|' || map[player.floor - 1][newY][newX].type == '_' || map[player.floor - 1][newY][newX].type == '.' || map[player.floor - 1][newY][newX].type == 'O') && !(newX == x && newY == y) && !enemyHit) {
        weapons_count[current_weapon] -= 1;
        map[player.floor - 1][newY][newX].display = 'w';
        map[player.floor - 1][newY][newX].color = 7;
    }
}



