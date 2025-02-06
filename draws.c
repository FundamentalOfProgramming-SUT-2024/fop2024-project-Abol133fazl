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

//Function to draw pillar
void drawpillar(WINDOW *win, Room room) {
    if (room.haspillar) {
        int xrnd = room.x + 1 + rand() % (room.width - 2);
        int yrnd = room.y + 1 + rand() % (room.height - 2);
        map[player.floor - 1][yrnd][xrnd].type = 'O';
        map[player.floor - 1][yrnd][xrnd].display = 'O';
        map[player.floor - 1][yrnd][xrnd].discovered = false;
    }
}

// Funtion to draw stairs
void drawStairs(Room *rooms, int roomCount, WINDOW *win) {
    int roomIndex = rand() % roomCount; // choose a random room
    Room selectedRoom = rooms[roomIndex];
    stair_x = selectedRoom.x + 1 + rand() % (selectedRoom.width - 2);
    stair_y = selectedRoom.y + 1 + rand() % (selectedRoom.height - 2);

    map[player.floor - 1][stair_y][stair_x].type = '>';
    map[player.floor - 1][stair_y][stair_x].display = '>';
    unimap[player.floor - 1][stair_y][stair_x] = '>';
    map[player.floor - 1][stair_y][stair_x].color = 3;
    map[player.floor - 1][stair_y][stair_x].discovered = false;
}

 // Function to draw traps
void drawTraps(Room *rooms, int roomCount, WINDOW *win) {
    for (int i = (rand() % roomCount - 3 + 1) + 2; i < roomCount; i++) {
        int roomIndex = rand() % roomCount; // choose a random room
        Room selectedRoom = rooms[roomIndex];
        trap_x = selectedRoom.x + 1 + rand() % (selectedRoom.width - 2);
        trap_y = selectedRoom.y + 1 + rand() % (selectedRoom.height - 2);

        map[player.floor - 1][trap_y][trap_x].type = '^';
        map[player.floor - 1][trap_y][trap_x].display = '.';
        map[player.floor - 1][trap_y][trap_x].color = 1;
        map[player.floor - 1][trap_y][trap_x].discovered = false;
    }
}

 // Function to draw golds
void drawGolds(Room *rooms, int roomCount, WINDOW *win) {
    for (int i = (rand() % roomCount - 3 + 1) + 2; i < roomCount; i++) {
        int roomIndex = rand() % roomCount; // choose a random room
        Room selectedRoom = rooms[roomIndex];
        gold_x = selectedRoom.x + 1 + rand() % (selectedRoom.width - 2);
        gold_y = selectedRoom.y + 1 + rand() % (selectedRoom.height - 2);
        int goldtype = rand() % 5;
        if (goldtype == 2) {
            map[player.floor - 1][gold_y][gold_x].color = 7;
            map[player.floor - 1][gold_y][gold_x].type = '/';
            map[player.floor - 1][gold_y][gold_x].display = '*';
            unimap[player.floor - 1][gold_y][gold_x] = L'★';
            map[player.floor - 1][gold_y][gold_x].discovered = false;
        }
        else {
            map[player.floor - 1][gold_y][gold_x].color = 4;
            map[player.floor - 1][gold_y][gold_x].type = '*';
            map[player.floor - 1][gold_y][gold_x].display = '*';
            unimap[player.floor - 1][gold_y][gold_x] = L'★';
            map[player.floor - 1][gold_y][gold_x].discovered = false;
        }
    }
}

// Function to draw weapons
void drawWeapons(Room *rooms, int roomCount, WINDOW *win) {
    for (int i = (rand() % roomCount - 3) + 2; i < roomCount; i++) {
        int roomIndex = rand() % roomCount; // choose a random room
        Room selectedRoom = rooms[roomIndex];
        int attempts = 0;
        while (attempts < 100000) {
            weapon_x = selectedRoom.x + 1 + rand() % (selectedRoom.width - 2);
            weapon_y = selectedRoom.y + 1 + rand() % (selectedRoom.height - 2);
            attempts++;
            if (map[player.floor - 1][weapon_y][weapon_x + 1].type == '.') break;
        }

        map[player.floor - 1][weapon_y][weapon_x].type = 'w';
        map[player.floor - 1][weapon_y][weapon_x].display = 'w';
        int type = rand() % 5;
        if (type == 0) {
            unimap[player.floor - 1][weapon_y][weapon_x] = L'⚔';
            map[player.floor - 1][weapon_y][weapon_x].color = 0;
        } else if (type == 1) {
            unimap[player.floor - 1][weapon_y][weapon_x] = L'🗡';
            map[player.floor - 1][weapon_y][weapon_x].color = 2;
        } else if (type == 2) {
            unimap[player.floor - 1][weapon_y][weapon_x] = L'⚚';
            map[player.floor - 1][weapon_y][weapon_x].color = 1;
        } else if (type == 3) {
            unimap[player.floor - 1][weapon_y][weapon_x] = L'➹';
            map[player.floor - 1][weapon_y][weapon_x].color = 5;
        } else if (type == 4) {
            unimap[player.floor - 1][weapon_y][weapon_x] = L'⚒';
            map[player.floor - 1][weapon_y][weapon_x].color = 3;
        }
        map[player.floor - 1][weapon_y][weapon_x].discovered = false;
    }
}
void displayWeaponsMenu() {
    int highlight = current_weapon;
    int ch;

    WINDOW *weaponWin = newwin(12, 60, 5, 35);
    box(weaponWin, 0, 0);
    keypad(weaponWin, true);
    mvwprintw(weaponWin, 0, 1, " Select a weapon to use ");
    while (1) {

        for (int i = 0; i < 5; i++) {
            if (i == highlight) {
                wattron(weaponWin, A_REVERSE);
            }
            mvwprintw(weaponWin, i + 1, 1, "%s: (x%d)    length:%d     type:%s", weapons_name[i], weapons_count[i], weapons_length[i], (i == 0 || i == 4) ? "Near" : "Far");
            wattroff(weaponWin, A_REVERSE);
        }

        wrefresh(weaponWin);

        ch = wgetch(weaponWin);
        switch (ch) {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = 5 - 1;
                break;

            case KEY_DOWN:
                highlight++;
                if (highlight >= 5) highlight = 0;
                break;

            case 10:
                if (weapons_count[highlight] > 0) {
                    current_weapon = highlight;
                    updateMessages(messageWin, "You set a new weapon!");
                    updateStatus(statusWin, &player);
                } else {
                    updateMessages(messageWin, "You have no more of this weapon!");
                }
                break;

            case 'q':
                delwin(weaponWin);
                refresh();
                return;
        }
    }
}

// Function to draw foods
void drawFoods(Room *rooms, int roomCount, WINDOW *win) {
    for (int i = (rand() % roomCount - 2); i < roomCount; i++) {
        int roomIndex = rand() % roomCount; // choose a random room
        Room selectedRoom = rooms[roomIndex];
        food_x = selectedRoom.x + 1 + rand() % (selectedRoom.width - 2);
        food_y = selectedRoom.y + 1 + rand() % (selectedRoom.height - 2);

        map[player.floor - 1][food_y][food_x].type = '~';
        map[player.floor - 1][food_y][food_x].display = '~';
         unimap[player.floor - 1][food_y][food_x] = L'♥';
        int type = rand() % 7;
        if (type == 0 || type == 1) {
            map[player.floor - 1][food_y][food_x].color = 6; // normal
        } else if (type == 2 || type == 3) {
            map[player.floor - 1][food_y][food_x].color = 1; // super
        } else if (type == 4 || type == 5) {
            map[player.floor - 1][food_y][food_x].color = 2; // magic
        } else if (type == 6) {
            map[player.floor - 1][food_y][food_x].color = 3; // corrupted
        }
        map[player.floor - 1][food_y][food_x].discovered = false;
    }
}
void displayFoodMenu() {
    int highlight = 0;
    int ch;

    WINDOW *foodWin = newwin(12, 40, 5, 20);
    box(foodWin, 0, 0);
    keypad(foodWin, true);
    mvwprintw(foodWin, 0, 1, " Select a Food to Eat ");
    while (1) {

        for (int i = 0; i < 4; i++) {
            if (i == highlight) {
                wattron(foodWin, A_REVERSE);
            }
            if (foods_count[i] > 100) foods_count[i] = 0;
            if (foods_count[i] < 0) foods_count[i] = 0;
            mvwprintw(foodWin, i + 1, 1, "%s (x %d)", foods_name[i], foods_count[i]);
            wattroff(foodWin, A_REVERSE);
        }
        

        mvwprintw(foodWin, 8, 1, "Press Enter to eat, q to exit.");
        wrefresh(foodWin);

        mvwprintw(foodWin, 6, 1, "Hunger: [");
        int maxWidth = 20;
        int filledWidth = (player.hunger * maxWidth) / 100;
        for (int i = 0; i < maxWidth; i++) {
            if (i < filledWidth) {
                wattron(foodWin, A_REVERSE);
                waddch(foodWin, ' ');
                wattroff(foodWin, A_REVERSE);
            } else {
                waddch(foodWin, ' ');
            }
        }
        waddstr(foodWin, "]");

        wrefresh(foodWin);

        ch = wgetch(foodWin);
        switch (ch) {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = 4 - 1;
                break;

            case KEY_DOWN:
                highlight++;
                if (highlight >= 4) highlight = 0;
                break;

            case 10:
                if (foods_count[highlight] > 0) {
                    foods_count[highlight] -= 1;
                    switch (highlight) {
                        case 0: player.hp = (player.hp < 80) ? (player.hp + 20) : 100; player.hunger = 0; updateMessages(messageWin, "You ate food and restored health!"); updateStatus(statusWin, &player); break;
                        case 1: player.hp = (player.hp < 80) ? (player.hp + 20) : 100; player.hunger = 0; updateMessages(messageWin, "You ate super food and you have more power now!"); updateStatus(statusWin, &player); damage_is_on = 2; damageTime = 10; break;
                        case 2: player.hp = (player.hp < 80) ? (player.hp + 20) : 100; player.hunger = 0; updateMessages(messageWin, "You ate magic food and you're faster now!"); updateStatus(statusWin, &player); speed_is_on = 1; speedTime = 10; break;
                        case 3: player.hp -= 10; updateMessages(messageWin, "You ate corrupted food and you lose some of your health"); updateStatus(statusWin, &player); break;
                    }
                } else {
                    updateMessages(messageWin, "You have no more of this food!");
                }
                break;

            case 'q':
                delwin(foodWin);
                refresh();
                return;
        }
    }
}

// Function to draw spells
void drawSpells(Room *rooms, int roomCount, WINDOW *win) {
    for (int i = (rand() % roomCount - 4); i < roomCount; i++) {
        int roomIndex = rand() % roomCount; // choose a random room
        Room selectedRoom = rooms[roomIndex];
        spell_x = selectedRoom.x + 1 + rand() % (selectedRoom.width - 2);
        spell_y = selectedRoom.y + 1 + rand() % (selectedRoom.height - 2);

        map[player.floor - 1][spell_y][spell_x].type = '%';
        map[player.floor - 1][spell_y][spell_x].display = '%';
        unimap[player.floor - 1][spell_y][spell_x] = L'▲';
        int type = rand() % 3;
        if (type == 0) {
            map[player.floor - 1][spell_y][spell_x].color = 10; // health
        } else if (type == 1) {
            map[player.floor - 1][spell_y][spell_x].color = 1; // speed
        } else if (type == 2) {
            map[player.floor - 1][spell_y][spell_x].color = 5; // damage
        }
        //map[player.floor - 1][spell_y][spell_x].color = 10;
        map[player.floor - 1][spell_y][spell_x].discovered = false;
    }
}
void displaySpellsMenu() {
    int highlight = 0;
    int ch;

    WINDOW *spellWin = newwin(12, 40, 5, 20);
    box(spellWin, 0, 0);
    keypad(spellWin, true);
    mvwprintw(spellWin, 0, 1, " Select a Spell to use ");
    while (1) {

        for (int i = 0; i < 3; i++) {
            if (i == highlight) {
                wattron(spellWin, A_REVERSE);
            }
            mvwprintw(spellWin, i + 1, 1, "%s (x%d)", spells_name[i], spells_count[i]);
            wattroff(spellWin, A_REVERSE);
        }

        wrefresh(spellWin);

        ch = wgetch(spellWin);
        switch (ch) {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = 4 - 1;
                break;

            case KEY_DOWN:
                highlight++;
                if (highlight >= 4) highlight = 0;
                break;

            case 10:
                if (spells_count[highlight] > 0) {
                    spells_count[highlight]--;
                    if (highlight == 0) {
                        player.hunger = 0;
                        player.hp = 100;
                        health_is_on = 1;
                        healthTime = 10;
                        updateMessages(messageWin, "You used spell and restored health!");
                        updateStatus(statusWin, &player);
                    } else if (highlight == 1) {
                        speed_is_on = 1;
                        speedTime = 10;
                        updateMessages(messageWin, "You used spell and increased your speed!");
                        updateStatus(statusWin, &player);
                    } else if (highlight == 2) {
                        damage_is_on = 2;
                        damageTime = 10;
                        updateMessages(messageWin, "You used spell and increased your damage!");
                        updateStatus(statusWin, &player);
                    } 
                } else {
                    updateMessages(messageWin, "You have no more of this spell!");
                }
                break;

            case 'q':
                delwin(spellWin);
                refresh();
                return;
        }
    }
}

// Function to draw hidden door
void addHiddenDoors(Room *rooms, int roomCount) {
    for (int i = 0; i < 2; i++) {
        Room *room = &rooms[rand() % roomCount];
        int numHiddenDoors = 1;

        for (int j = 0; j < numHiddenDoors; j++) {
            int side = rand() % 4;
            int x, y;

            switch (side) {
                case 0: // up wall
                    x = room->x + 1 + rand() % (room->width - 2);
                    y = room->y;
                    break;
                case 1: // down wall
                    x = room->x + 1 + rand() % (room->width - 2);
                    y = room->y + room->height - 1;
                    break;
                case 2: // left wall
                    x = room->x;
                    y = room->y + 1 + rand() % (room->height - 2);
                    break;
                case 3: // right wall
                    x = room->x + room->width - 1;
                    y = room->y + 1 + rand() % (room->height - 2);
                    break;
            }

            map[player.floor - 1][y][x].type = '?';
            map[player.floor - 1][y][x].discovered = false;
            map[player.floor - 1][y][x].color = 1;
        }
    }
}

// Function to draw windows
void drawWindows(Room *rooms, int roomCount, WINDOW *win) {
    int windowsToAdd = (rand() % 2) + 2;
    
    for (int i = 0; i < windowsToAdd; i++) {
        int roomIndex = rand() % roomCount;
        Room *room = &rooms[roomIndex];

        int wallSide = rand() % 4;
        int wx, wy;
        
        switch (wallSide) {
            case 0:
                wx = room->x + 1 + rand() % (room->width - 2);
                wy = room->y;
                break;
            case 1:
                wx = room->x + 1 + rand() % (room->width - 2);
                wy = room->y + room->height - 1;
                break;
            case 2:
                wx = room->x;
                wy = room->y + 1 + rand() % (room->height - 2);
                break;
            case 3:
                wx = room->x + room->width - 1;
                wy = room->y + 1 + rand() % (room->height - 2);
                break;
        }

        if (map[player.floor - 1][wy][wx].type == '_' || map[player.floor - 1][wy][wx].type == '|') {
            map[player.floor - 1][wy][wx].type = '=';
            map[player.floor - 1][wy][wx].display = '=';
            map[player.floor - 1][wy][wx].color = 1;
        }
    }
}

// Function to draw corridors
void drawCorridor(WINDOW *win, int x1, int y1, int x2, int y2, Room *rooms, int roomCount) {
    int x = x1, y = y1;

    while (x != x2 || y != y2) {
        if (!isInsideRoom(x, y, rooms, roomCount)) {
            map[player.floor - 1][y][x].type = '#';
            map[player.floor - 1][y][x].display = '#';
            map[player.floor - 1][y][x].color = 1;
            map[player.floor - 1][y][x].discovered = false;
        }
        if (x != x2) x += (x < x2) ? 1 : -1;
        if (y != y2) y += (y < y2) ? 1 : -1;
    }
}

// Function to making door
void makedoor(int x, int y, Room *rooms, int roomCount) {

    char ch1 = map[player.floor - 1][y + 1][x].type;
    char ch2 = map[player.floor - 1][y - 1][x].type;
    char ch3 = map[player.floor - 1][y][x + 1].type;
    char ch4 = map[player.floor - 1][y][x - 1].type;
    char ch5 = map[player.floor - 1][y + 1][x + 1].type;
    char ch6 = map[player.floor - 1][y - 1][x - 1].type;
    char ch7 = map[player.floor - 1][y - 1][x + 1].type;
    char ch8 = map[player.floor - 1][y + 1][x - 1].type;

    if (isInsideRoom(x, y, rooms, roomCount)) {

        if (ch1 == '#' || ch2 == '#' || ch3 == '#' || ch4 == '#' || ch5 == '#' || ch6 == '#' || ch7 == '#' || ch8 == '#') {

            map[player.floor - 1][y][x].type = '+';
            map[player.floor - 1][y][x].display = '+';
            map[player.floor - 1][y][x].color = 1;
            map[player.floor - 1][y][x].discovered = false;
        }
    }
}