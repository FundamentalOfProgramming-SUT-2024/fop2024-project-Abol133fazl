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

// Function to play music
const char *music_list[] = {
    "1.mp3", "2.mp3", "3.mp3", "4.mp3"
};
const int music_count = 4;
Mix_Music *current_music = NULL;
int current_music_index = 0;
void play_music(int index) {
    if (!music_enabled || index < 0 || index >= music_count) return;

    if (current_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(current_music);
    }

    current_music = Mix_LoadMUS(music_list[index]);
    if (!current_music) {
        printf("Failed to load %s: %s\n", music_list[index], Mix_GetError());
        return;
    }
    Mix_PlayMusic(current_music, -1);
}
void detect_music() {
    if (cur_theme == 1 && last_theme != 1) {
        play_music((0 + (default_music - 1)) % 4);
    } else if (cur_theme == 8 && cur_theme != last_theme) {
        play_music((1 + (default_music - 1)) % 4);
    } else if (cur_theme == 10 && cur_theme != last_theme) {
        play_music((2 + (default_music - 1)) % 4);
    } else if (cur_theme == 11 && cur_theme != last_theme) {
        play_music((3 + (default_music - 1)) % 4);
    }
}