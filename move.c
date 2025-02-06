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

// check if can move to cell or not
bool canMoveTo(int x, int y) {

    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }

    char destinationType = map[player.floor - 1][y][x].type;
    char destinationDisplay = map[player.floor - 1][y][x].display;
    // checking cell typr
    if (destinationType == '%' || (strchr("#+.^>?=&8*~w/DFGSU", destinationType) != NULL) || (strchr("#+.^>?8*~w/DFGSU", destinationDisplay) != NULL)) {
        return true;
    }

    return false;
}

// Function to move player
void movePlayer(int dx, int dy) {
    if (speed_is_on && canMoveTo(player_x + (dx * 2), player_y + (dy * 2))) {
        dx *= 2;
        dy *= 2;
    }
    int new_x = player_x + dx;
    int new_y = player_y + dy;
    if (canMoveTo(new_x, new_y)) {
        mvwaddch(mapWin, player_y, player_x, last_char);
        map[player.floor - 1][player_y][player_x].display = last_char;
        map[player.floor - 1][player_y][player_x].type = last_char;
        unimap[player.floor - 1][player_y][player_x] = last_uni;
        map[player.floor - 1][player_y][player_x].color = last_color;

        player_x += dx;
        player_y += dy;
    
        last_char = map[player.floor - 1][player_y][player_x].type;
        last_uni = unimap[player.floor - 1][player_y][player_x];
        last_color = map[player.floor - 1][player_y][player_x].color;

        Room *room = find_room(player_x, player_y);

        /////////////////////////// check by display
        if (map[player.floor - 1][player_y][player_x].display == 'w' && map[player.floor - 1][player_y][player_x].color == 7) {
            weapons_count[current_weapon] += 1;
            updateMessages(messageWin, "You recollected your weapon!");
            map[player.floor - 1][player_y][player_x].color = 1;
            last_color = 1;
        }
        if (map[player.floor - 1][player_y][player_x].display == 'D' || map[player.floor - 1][player_y][player_x].display == 'F') {
            last_char = (map[player.floor - 1][player_y][player_x].display == 'D') ? 'D' : 'F';
        }

        map[player.floor - 1][player_y][player_x].display = 'X';
        map[player.floor - 1][player_y][player_x].color = (character_color == 1) ? 0 : 3;




        //////////////////////////////////// check by type
        if (last_char == '.') {
            updateMessages(messageWin, "You are in a room!");
            if (room->theme != 11) {
                discoverRoom(mapWin, room);
                handleEnemiesInRoom(room);
            } else {
                map[player.floor][player_y][player_x].discovered = true;
                for (int i = 0; i <= 2; i++) {
                    for (int j = 0; j <= 2; j++) {
                        map[player.floor - 1][player_y + i][player_x + j].discovered = true;
                        map[player.floor - 1][player_y + i][player_x - j].discovered = true;
                        map[player.floor - 1][player_y - i][player_x + j].discovered = true;
                        map[player.floor - 1][player_y - i][player_x - j].discovered = true;
                    }
                }
            }
            last_theme = cur_theme;
            cur_theme = room->theme;
        } else if (last_char == '#') {
            last_theme = cur_theme;
            cur_theme = 0;
            nightmare_is_on = false;
            updateMessages(messageWin, "You are in a corridor!");
        } else if (last_char == '+') {
            updateMessages(messageWin, "You found a door!");
            if (room->theme != 11) {
                discoverRoom(mapWin, room); //discover room
            } else {
                map[player.floor][player_y][player_x].discovered = true;
                for (int i = 0; i <= 2; i++) {
                    for (int j = 0; j <= 2; j++) {
                        map[player.floor - 1][player_y + i][player_x + j].discovered = true;
                        map[player.floor - 1][player_y + i][player_x - j].discovered = true;
                        map[player.floor - 1][player_y - i][player_x + j].discovered = true;
                        map[player.floor - 1][player_y - i][player_x - j].discovered = true;
                    }
                }
            }
        } else if (last_char == '>') {
            updateMessages(messageWin, "You found stairs to the next level! Next level (>) or previous level (<)?");
            char op = getch();
            if (op == '>') {
                saveGame("PrevLevel");
                if (prevlevel) loadGame("CurrentLevel");
                else {
                    clear(); 
                    player.floor++;
                    last_char = '.';
                    last_uni = L'.';
                    last_color = 0;
                    generate_map();
                }
                updateMessages(messageWin, "You are in new level");
            } else if (op == '<') {
                prevlevel = true;
                saveGame("CurrentLevel");
                loadGame("PrevLevel");
                updateMessages(messageWin, "You are in previous level");
            }
        } else if (last_char == '^') {
                updateMessages(messageWin, "You found trap!");
                if (room->theme == 8) {
                    player.hp -= 5;
                    updateStatus(statusWin, &player);
                } else {
                    last_char = '.';
                    last_color = 0;
                    usleep(2000000);
                    saveGame("CurrentLevel");
                    generate_battle_map();
                }
        } else if (last_char == '*' || last_char == '/') {
            if (!g_is_on) {    
                if (map[player.floor - 1][player_y][player_x].type == '*') {
                    updateMessages(messageWin, "You found some gold!");
                    player.gold += 10;
                    updateStatus(statusWin, &player);
                    map[player.floor - 1][player_y][player_x].color = 4;
                } else {
                    updateMessages(messageWin, "You found some black gold!");
                    player.black_gold += 5;
                    updateStatus(statusWin, &player);
                    map[player.floor - 1][player_y][player_x].color = 7;
                }
                last_char = '.';
                last_uni = L'.';
                last_color = 0;
                }
        } else if (last_char == 'w') {
            if (!g_is_on) {
                updateMessages(messageWin, "You found a weapon! If you want to claim it ot not press Y/N");
                int claim = getch();
                if (claim == 'y') {
                    int newOrNot = (map[player.floor - 1][player_y][player_x].color == 7);
                    int type = last_color;
                    WINDOW *weaponWin = newwin(5, 40, 10, 30);
                    box(weaponWin, 0, 0);
                    if (type == 0) {
                        mvwprintw(weaponWin, 2, 1, "You found a sword! %lc", L'⚔');
                        weapons_count[0] = 1;
                    } else if (type ==2) {
                        mvwprintw(weaponWin, 2, 1, "You found a dager! %lc", L'🗡');
                        weapons_count[1] += (newOrNot) ? 1 : 10;
                    } else if (type == 1) {
                        mvwprintw(weaponWin, 2, 1, "You found a Magic Wand! %lc", L'🪄');
                        weapons_count[2] =+ (newOrNot) ? 1 : 8;
                    } else if (type == 5) {
                        mvwprintw(weaponWin, 2, 1, "You found a Normal Arrow! %lc", L'➳');
                        weapons_count[3] += (newOrNot) ? 1 : 20;
                    } else {
                        mvwprintw(weaponWin, 2, 1, "You found a Mace! %lc", L'⚒');
                    }

                    map[player.floor - 1][player_y][player_x].color = 5;
                    last_char = '.';
                    last_uni = L'.';
                    last_color = 0;
                    wrefresh(weaponWin);
                    getch();
                    delwin(weaponWin);
                }
            }
        } else if (last_char == '~') {
            if (!g_is_on) {
                updateMessages(messageWin, "You found some food!");
                int type;
                if (last_color == 6) type = 0;
                if (last_color == 1) type = 1;
                if (last_color == 2) type = 2;
                if (last_color == 3) type = 3;
                foods_count[type] += 1;
                if (foods_count[type] > 5) foods_count[type] = 5;
                if (foods_count[type] < 0) foods_count[type] = 0;
                updateStatus(statusWin, &player);
                map[player.floor - 1][player_y][player_x].color = last_color;
                last_char = '.';
                last_uni = L'.';
                last_color = 0;
            }
        } else if (last_char == '8') {
            updateMessages(messageWin, "!!!!!You Won!!!!"); getch(); last_char = '.'; last_uni = L'.'; last_color = 0; clear(); generate_treasure_map();
        } else if (last_char == '=') {
            updateMessages(messageWin, "You opened a window!");
            all_map_discovered = true;
            printMap(mapWin);
            usleep(5000000);
            all_map_discovered = false;
            printMap(mapWin);
        } else if (last_char == '?') {
            updateMessages(messageWin, "You found a hidden door");
            //createSpellRoom();
            last_char = '?';
        } else if (last_char == '%') {
            if (!g_is_on) {
                updateMessages(messageWin, "You found a spell! If you want to claim it ot not press Y/N");
                int claim = getch();
                if (claim == 'y') {
                    int typespell;
                    if (last_color == 10) {
                        typespell = 0;
                    } else if (last_color == 1) {
                        typespell = 1;
                    } else if (last_color == 5) {
                        typespell = 2;
                    }
                    spells_count[typespell] += 1;
                    updateMessages(messageWin, "You found a spell");
                    map[player.floor - 1][player_y][player_x].color = 10;
                    last_char = '.';
                    last_uni = L'.';
                }
            }
        } else if (last_char == 'D' || last_char == 'F') {
            last_char = (last_char == 'D') ? 'D' : 'F';
            for (int i = 0; i < roomCount; i++) {
                    Room *room = &rooms[i];
                    if (player_x >= room->x && player_x < room->x + room->width &&
                        player_y >= room->y && player_y < room->y + room->height) {
                        handleEnemiesInRoom(room);
                        break;
                    }
                }
        } else if (last_char == '&') {
            if (last_color == 1 || last_color == 0) {
                loadGame("CurrentLevel");
                last_char = '^';
                last_uni = L'^';
                last_color = 3;
            } else {
                player.floor = 1;
                player.score += 20000;
                delwin(messageWin);
                delwin(mapWin);
                delwin(statusWin);
                clear();
                WINDOW *congrats = newwin(MAP_HEIGHT + 2, MAP_WIDTH + 2, 3, 0);
                attron(COLOR_PAIR(3));
                show_win_message();
                mvprintw(LINES - 3, 65, "Score: %d", player.score);
                attroff(COLOR_PAIR(3));
                update_scoreboard("scores.txt");
                getch();
                delwin(congrats);
                Mix_HaltMusic();
                Mix_FreeMusic(current_music);
                Mix_CloseAudio();
                endwin();
                clear();
                display_scoreboard();
                refresh();
                exit(0);
            }
        }

        detect_music(room);
        
        // discover corridors
        map[player.floor - 1][player_y][player_x].discovered = true;
        for (int i = 0; i <= 2; i++) {
            for (int j = 0; j <= 2; j++) {
                if (map[player.floor - 1][player_y + i][player_x + j].type == '#')
                    map[player.floor - 1][player_y + i][player_x + j].discovered = true;
                if (map[player.floor - 1][player_y + i][player_x - j].type == '#')
                    map[player.floor - 1][player_y + i][player_x - j].discovered = true;
                if (map[player.floor - 1][player_y - i][player_x + j].type == '#')
                    map[player.floor - 1][player_y - i][player_x + j].discovered = true;
                if (map[player.floor - 1][player_y - i][player_x - j].type == '#')
                    map[player.floor - 1][player_y - i][player_x - j].discovered = true;
            }
        }

        // increase hunger
        player.hunger += (game_difficulty - 1 == 0) ? 0 : (game_difficulty - 1 == 1) ? 3 : 5; 
        if (player.hunger > 100) player.hunger = 100;

        // decrease or increase hp
        if (player.hunger > 70) {
            player.hp -= 1;
            if (player.hp < 0) player.hp = 0;
            updateStatus(statusWin, &player);
        } else {
            player.hp += (health_is_on) ? 2 : 1;
            if (player.hp > 100) player.hp = 100;
            updateStatus(statusWin, &player);
        }

        // check for themes applications
        if (cur_theme == 10) {
            player.hp -= 3;
            if (player.hp < 0) player.hp = 0;
            updateStatus(statusWin, &player);
        }
        if (cur_theme == 11) {
            nightmare_is_on = true;
        }

        // spells and foods application
        if (speedTime > 0) {
            speedTime--;
        } else if (speedTime == 0) {
            speed_is_on = 0;
        }
        if (healthTime > 0) {
            healthTime--;
        } else if (healthTime == 0) {
            health_is_on = 0;
        }
        if (damageTime > 0) {
            damageTime--;
        } else if (damageTime == 0) {
            damage_is_on = 0;
        }

        // check for food timer
        if (change_food_timer == 0) {
            foods_count[3] += foods_count[0];
            foods_count[0] = foods_count[1] + foods_count[2];
            foods_count[1] = 0;
            foods_count[2] = 0;
            change_food_timer = 20;
        } else {
            change_food_timer -= 1;
        }

        player.score = (player.black_gold * 20) + player.gold + (player.kills * 20);
        updateStatus(statusWin, &player);

        // update map
        printMap(mapWin);
    } else {
        updateMessages(messageWin, "You can't move there!");
    }
}

// Function for User Movements
void get_move_from_user() {
    int ch;
    while ((ch = getch()) != 'q') {
        if (player.hp <= 0) {
            player.floor = 1;
            player.hp = 100;
            delwin(messageWin);
            delwin(mapWin);
            delwin(statusWin);
            clear();
            WINDOW *lost_win = newwin(MAP_HEIGHT + 2, MAP_WIDTH + 2, 3, 0);
            attron(COLOR_PAIR(3));
            show_lose_message();
            mvprintw(LINES - 3, 65, "Score: %d", player.score);
            attroff(COLOR_PAIR(3));
            getch();
            delwin(lost_win);
            Mix_HaltMusic();
            Mix_FreeMusic(current_music);
            Mix_CloseAudio();
            endwin();
            clear();
            refresh();
            exit(0);
            break;
        } //lose in 0 hp
        switch (ch) {
            case '8': movePlayer(0, -1); break;
            case '2': movePlayer(0, 1); break;
            case '4': movePlayer(-1, 0); break;
            case '6': movePlayer(1, 0); break;
            case '7': movePlayer(-1, -1); break;
            case '9': movePlayer(1, -1); break;
            case '1': movePlayer(-1, 1); break;
            case '3': movePlayer(1, 1); break;
            case 'f': {  // auto move
                updateMessages(messageWin, "Press a direction for auto-move.");
                int dir = getch();
                int dx = 0, dy = 0;
                switch (dir) {
                    case '8': dx = 0; dy = -1; break;
                    case '2': dx = 0; dy = 1; break;
                    case '4': dx = -1; dy = 0; break;
                    case '6': dx = 1; dy = 0; break;
                    case '7': dx = -1; dy = -1; break;
                    case '9': dx = 1; dy = -1; break;
                    case '1': dx = -1; dy = 1; break;
                    case '3': dx = 1; dy = 1; break;
                }

                if (dx != 0 || dy != 0) {
                    while (canMoveTo(player_x + dx, player_y + dy)) {
                        movePlayer(dx, dy);
                    }
                    updateMessages(messageWin, "Auto-move stopped!");
                }
                break;
            }
            case '~': clear(); last_char = '.'; generate_map(); break;
            case 'm': all_map_discovered = !all_map_discovered; printMap(mapWin); break;
            case 'e': displayFoodMenu(); break;
            case 'c': displaySpellsMenu(); break;
            case 'w': displayWeaponsMenu(); break;
            case ' ': {
                Room *room = find_room(player_x, player_y);
                if (current_weapon == 0 || current_weapon == 4)
                    hitNearEnemies(room, player_x, player_y);
                else
                    hitFarEnemies(room, player_x, player_y);
            } break;
            case 'g': g_is_on = !g_is_on; break;
            case 's': saveGame(""); break;
            case 'l': loadGame(""); break;
        }
    }
}