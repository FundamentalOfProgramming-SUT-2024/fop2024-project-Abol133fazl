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

// Function to save the game
void saveGame(char *input) {

    mkdir("saves", 0777);

    char filePath[100];
    snprintf(filePath, sizeof(filePath), "saves/%s%s.sav", player_username, input);

    FILE *file = fopen(filePath, "wb");
    if (!file) {
        updateMessages(messageWin, "Failed to save game");
        return;
    }

    fwrite(&player, sizeof(PlayerStatus), 1, file);
    fwrite(&inventory, sizeof(Inventory), 1, file);
    fwrite(map, sizeof(map), 1, file);
    fwrite(unimap, sizeof(unimap), 1, file);
    fwrite(&roomCount, sizeof(int), 1, file);
    fwrite(rooms, sizeof(rooms), 1, file);
    fwrite(&player_x, sizeof(int), 1, file);
    fwrite(&player_y, sizeof(int), 1, file);
    fwrite(&default_music, sizeof(int), 1, file);

    fclose(file);  // close file
    updateMessages(messageWin, "Game saved successfully!");
}
// funtion to load the saved game
void loadGame(char *input) {
    char filePath[100];
    snprintf(filePath, sizeof(filePath), "saves/%s%s.sav", player_username, input);

    FILE *file = fopen(filePath, "rb");
    if (!file) {
        updateMessages(messageWin, "No saved game found!");
        return;
    }
    
    fread(&player, sizeof(PlayerStatus), 1, file);
    fread(&inventory, sizeof(Inventory), 1, file);
    fread(map, sizeof(map), 1, file);
    fread(unimap, sizeof(unimap), 1, file);
    fread(&roomCount, sizeof(int), 1, file);
    fread(rooms, sizeof(rooms), 1, file);
    fread(&player_x, sizeof(int), 1, file);
    fread(&player_y, sizeof(int), 1, file);
    fread(&default_music, sizeof(int), 1, file);

    fclose(file);
    updateMessages(messageWin, "Game loaded successfully!");
    printMap(mapWin);
    updateStatus(statusWin, &player);
}