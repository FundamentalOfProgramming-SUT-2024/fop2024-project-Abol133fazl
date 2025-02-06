#include "menus.h"
#include "makemap.h"
#include "enemies.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <locale.h>

char *player_username;
int game_difficulty = 1; // 1 = Easy, 2 = Medium, 3 = Hard
int character_color = 1; // 1 = white, 2 = red
bool music_enabled = true;
int default_music = 1;

// Function to display main_menu
void display_menu(WINDOW *menu_win, int highlight, char *main_menu[], int n_main_menu) {
    int x = 2, y = 2;
    box(menu_win, 0, 0);
    for (int i = 0; i < n_main_menu; i++) {
        if (highlight == i) {
            wattron(menu_win, A_REVERSE);
        }
        mvwprintw(menu_win, y, x, "%s", main_menu[i]);
        wattroff(menu_win, A_REVERSE);
        y++;
    }
    wrefresh(menu_win);
}

// Function to create a new user
void create_new_user() {
    char username[30], password[30], email[50];
    echo();

    while (1) {
        clear();
        mvprintw(5, 10, "Enter Username: ");
        getstr(username);
        
        // check if username exists
        FILE *file = fopen("users.txt", "r");
        char existing_username[30], existing_password[30], existing_email[50];
        int username_exists = 0;

        if (file != NULL) {
            while (fscanf(file, "%s %s %s", existing_username, existing_password, existing_email) != EOF) {
                if (strcmp(username, existing_username) == 0) {
                    username_exists = 1;
                    break;
                }
            }
            fclose(file);
        }

        if (username_exists) {
            mvprintw(6, 10, "Username already exists. Press any key to retry...");
            getch();
            clrtoeol();
            refresh();
            continue;
        }

        move(6, 10);
        clrtoeol();
        break;
    }

    while (1) {
        mvprintw(7, 10, "Enter Password or press 'r' for random password: ");
        clrtoeol();
        getstr(password);

        if (strcmp(password, "r") == 0) {
        generate_random_password(password, 12);
        mvprintw(8, 10, "Generated Password: %s", password);
        refresh();
        break;
    }

        // check pass conditions
        int has_upper = 0, has_lower = 0, has_digit = 0;
        if (strlen(password) < 7) {
            mvprintw(8, 10, "Password must be at least 7 characters long. Press any key to retry...");
            getch();
            clrtoeol();
            refresh();
            continue;
        }
        for (int i = 0; i < strlen(password); i++) {
            if (isdigit(password[i])) has_digit = 1;
            else if (isupper(password[i])) has_upper = 1;
            else if (islower(password[i])) has_lower = 1;
        }
        if (!has_upper || !has_lower || !has_digit) {
            mvprintw(8, 10, "Password must include at least 1 uppercase, 1 lowercase, and 1 digit. Press any key to retry...");
            getch();
            clrtoeol();
            refresh();
            continue;
        }

        move(8, 10);
        clrtoeol();
        break;
    }

    while (1) {
        mvprintw(9, 10, "Enter Email: ");
        getstr(email);

        // check email format
        char *at = strchr(email, '@');
        char *dot = strrchr(email, '.');
        if (at == NULL || dot == NULL || at > dot || at == email || dot - at <= 1) {
            mvprintw(10, 10, "Invalid email format. Press any key to retry...");
            getch();
            clrtoeol();
            refresh();
            continue;
        }

        move(10, 10);
        clrtoeol();
        break;
    }

    // save user infs
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        mvprintw(11, 10, "Error opening file!");
        getch();
        return;
    }
    fprintf(file, "%s %s %s\n", username, password, email);
    fclose(file);

    player_username = username;

    mvprintw(11, 10, "User created successfully! Press any key to go to the pre_game menu...");
    getch();
    clear();
    refresh();
    noecho();
    pre_game_menu();
    return;
}
void generate_random_password(char *password, int length) {  // Function to generate random password
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";
    int charset_size = strlen(charset);

    srand(time(NULL));

    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % charset_size];
    }
    password[length] = '\0';
}

// Function to enter user
void enter_user() {
    char username[50];
    char password[50];
    char stored_username[50], stored_password[50], stored_email[100];
    FILE *file;
    
    echo();

    while (1) {
        clear();
        mvprintw(5, 10, "Enter your username (or press 'g' for login as a guest): ");
        getstr(username);

        if (strcmp(username, "g") == 0)
        {
            player_username = "guest";
            clear();
            refresh();
            noecho();
            pre_game_menu();
            return;
        }
        // open file to authenticate user
        file = fopen("users.txt", "r");
        if (!file) {
            mvprintw(6, 10, "Error: Unable to open user database.");
            refresh();
            getch();
            clear();
            return;
        }

        int user_found = 0;
        while (fscanf(file, "%s %s %s", stored_username, stored_password, stored_email) != EOF) {
            if (strcmp(username, stored_username) == 0) {
                user_found = 1;
                break;
            }
        }
        fclose(file);

        if (!user_found) {
            mvprintw(6, 10, "Error: Username not found.");
            mvprintw(7, 10, "Press any key to try again.");
            refresh();
            getch();
            continue;
        } else {
            break;
        }
    }

    while (1) {
        mvprintw(7, 10, "Enter your password (or press 'f' for Forgot Password): ");
        getstr(password);

        if (strcmp(password, "f") == 0) { // forgot password
            clear();
            mvprintw(5, 10, "Forgot Password Selected!");
            mvprintw(6, 10, "Enter your registered email: ");
            char input_email[100];
            getstr(input_email);

            // check email
            if (strcmp(input_email, stored_email) == 0) {
                char new_password[50];
                mvprintw(7, 10, "Enter your new password: ");
                getstr(new_password);

                // check new pass conditions
                if (strlen(new_password) < 7) {
                    mvprintw(8, 10, "Error: Password must be at least 7 characters.");
                    refresh();
                    getch();
                    continue;
                }

                // updating file
                FILE *temp = fopen("temp_users.txt", "w");
                FILE *original = fopen("users.txt", "r");
                while (fscanf(original, "%s %s %s", stored_username, stored_password, stored_email) != EOF) {
                    if (strcmp(username, stored_username) == 0) {
                        fprintf(temp, "%s %s %s\n", username, new_password, stored_email);
                    } else {
                        fprintf(temp, "%s %s %s\n", stored_username, stored_password, stored_email);
                    }
                }
                fclose(original);
                fclose(temp);
                remove("users.txt");
                rename("temp_users.txt", "users.txt");

                mvprintw(9, 10, "Password successfully reset!");
                clear();
                refresh();
                getch();
                noecho();
                return;
            } else {
                mvprintw(7, 10, "Error: Email does not match!");
                mvprintw(8, 10, "Press any key to try again.");
                clear();
                refresh();
                getch();
                continue;
            }
        } else {
            // check pass authenticate
            if (strcmp(password, stored_password) == 0) {
                player_username = username;
                mvprintw(8, 10, "Login successful!");
                getch();
                clear();
                refresh();
                noecho();
                pre_game_menu();
                return;
            } else {
                mvprintw(8, 10, "Error: Incorrect password.");
                mvprintw(9, 10, "Press any key to try again.");
                getch();
                clrtoeol();
                move(8, 10);
                clrtoeol();
                refresh();
                continue;
            }
        }
    }
    return;
}

// Function to display pre-game menu 
void pre_game_menu() {
    setlocale(LC_ALL, "");
    keypad(stdscr, TRUE);
    char *game_options[] = { 
        "🆕 New Game", 
        "🎮 Continue Game", 
        "🏆 Scoreboard", 
        "⚙️ Options", 
        "👤 Profile", 
        "🚪 Exit to Main Menu" 
    };
    int n_game_options = sizeof(game_options) / sizeof(char *);
    int highlight = 0;
    
    int width = 50;
    int height = 15;
    int startx = (COLS - width) / 2;
    int starty = (LINES - height) / 2;
    WINDOW *game_menu_win = newwin(height, width, starty, startx);
    
    init_pair(13, COLOR_WHITE, COLOR_YELLOW);
    init_pair(14, COLOR_WHITE, COLOR_BLUE);
    

    keypad(game_menu_win, TRUE);
    curs_set(0);
    wtimeout(game_menu_win, -1);
    
    while (1) {
        werase(game_menu_win);
        wattron(game_menu_win, COLOR_PAIR(5) | A_BLINK);
        wborder(game_menu_win, 
            '|', '|',
            '=', '=',
            '+', '+',
            '+', '+'
        );
        wattroff(game_menu_win, COLOR_PAIR(5) | A_BLINK);
        
        wattron(game_menu_win, COLOR_PAIR(5) | A_BLINK);
        mvwprintw(game_menu_win, 1, (width - 24)/2, "🚀 ROGUE 🚀");
        wattroff(game_menu_win, COLOR_PAIR(5) | A_BLINK);
        
        for (int i = 0; i < n_game_options; i++) {
            if (highlight == i) {
                wattron(game_menu_win, COLOR_PAIR(13) | A_BOLD);
                mvwaddstr(game_menu_win, i + 4, 5, "👉 "); // استفاده از کاراکترهای wide
                wprintw(game_menu_win, "%s", game_options[i]);
                wattroff(game_menu_win, COLOR_PAIR(13) | A_BOLD);
            } else {
                wattron(game_menu_win, COLOR_PAIR(14));
                mvwaddstr(game_menu_win, i + 4, 8, "⦿ "); 
                wprintw(game_menu_win, "%s", game_options[i]);
                wattroff(game_menu_win, COLOR_PAIR(14));
            }
        }
        
        mvwprintw(game_menu_win, height-2, 2, 
            "▲ ▼ Navigate | ENTER Select | Q Quit"
        );
        
        wrefresh(game_menu_win);
        
        int choice = wgetch(game_menu_win);

        if (choice == KEY_UP) {
            if (highlight == 0) {
                highlight = n_game_options - 1;
            } else {
                highlight--;
            }
        } else if (choice == KEY_DOWN) {
            if (highlight == n_game_options - 1) {
                highlight = 0;
            } else {
                highlight++;
            }
        } else if (choice == 10) { // Enter key
            if (highlight == 0) { // new game
                clear();
                mvprintw(5, 10, "Creating a new game...");
                refresh();
                getch();
                display_map();
                break;
            } else if (highlight == 1) { // continue game
                clear();
                mvprintw(5, 10, "Continuing the previous game...");
                refresh();
                display_prev_game();
                getch();
                break;
            } else if (highlight == 2) {
                clear();
                refresh();
                display_scoreboard();
            } else if (highlight == 3) {
                clear();
                refresh();
                display_options();
            } else if (highlight == 4) {
                clear();
                refresh();
                profile_menu();
            } else if (highlight == 5) { // return to main menu
                clear();
                mvprintw(5, 10, "Returning to the main menu...");
                clear();
                refresh();
                getch();
                break;
            }
        } else if (choice == 'q') {
            clear();
            refresh();
            break;
        }
    }

    delwin(game_menu_win); // delete geme menu display
    refresh();
}

// Function to display scoreboard
void display_scoreboard() {
    Player players[MAX_PLAYERS];
    int n_players = read_scores("scores.txt", players, MAX_PLAYERS);
    
    if (n_players == 0) {
        mvprintw(5, 10, "No players found in the scoreboard.");
        refresh();
        getch();
        return;
    }

    for (int i = 0; i < n_players - 1; i++) {
        for (int j = i + 1; j < n_players; j++) {
            if (players[i].total_points < players[j].total_points) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    int current_page = 0;
    int total_pages = (n_players + PLAYERS_PER_PAGE - 1) / PLAYERS_PER_PAGE;

    int ch;
    do {
        clear();
        mvprintw(1, (COLS - 20) / 2, "🏆 Scoreboard 🏆");
        mvprintw(3, 2, 
            "╔═════╦══════════════╦════════════╦══════════╦══════════╦══════════════╦════════════╗"
        );
        mvprintw(4, 2, 
            "║Rank ║    Username  ║ TotalScore ║ Gold     ║ Games    ║ First Game   ║ Nickname   ║"
        );
        mvprintw(5, 2, 
            "╠═════╬══════════════╬════════════╬══════════╬══════════╬══════════════╬════════════╣"
        );

        int start_index = current_page * PLAYERS_PER_PAGE;
        int end_index = start_index + PLAYERS_PER_PAGE;
        if (end_index > n_players) end_index = n_players;

        for (int i = start_index; i < end_index; i++) {
            int color_pair = 0;
            const char *trophy = "";
            const char *nick = "";

            if (i == 0) { 
                color_pair = 1;
                trophy = "🏆";
                nick = "El DORO";
            } else if (i == 1) { 
                color_pair = 2;
                trophy = "🥈";
                nick = "DEL TORO";
            } else if (i == 2) { 
                color_pair = 3;
                trophy = "🥉";
                nick = "GEL BORO";
            }

            switch(i) {
            case 0: trophy = "🎖️"; nick = "El DORO"; color_pair = 4; break;
            case 1: trophy = "🥈"; nick = "DEL TORO"; color_pair = 1; break;
            case 2: trophy = "🥉"; nick = "GEL BOR"; color_pair = 3; break;
            default: trophy = "🔹";
        }

            if (strcmp(players[i].username, player_username) == 0) {
                color_pair = 6;
            }

            if (color_pair != 0) {
                attron(COLOR_PAIR(color_pair) | A_BOLD | A_REVERSE);
            }

            mvprintw(6 + (i - start_index), 2, 
                "║%2d %s║%-14s║%11d ║%9d ║%8d  ║%-12s  ║%-10s  ║",
                i+1, trophy, players[i].username, 
                players[i].total_points, players[i].gold_points, 
                players[i].games_completed, players[i].first_game_time, nick
            );

            if (color_pair != 0) {
                attroff(COLOR_PAIR(color_pair) | A_BOLD | A_REVERSE);
            }
        }

        mvprintw(LINES - 2, 2, "Page %d/%d - Press 'n' for next, 'p' for previous, 'q' to quit", current_page + 1, total_pages);
        refresh();
        mvprintw(15, 2, 
            "╚═════╩══════════════╩════════════╩══════════╩══════════╩══════════════╩════════════╝"
        );

        ch = getch();
        if (ch == 'n' && current_page < total_pages - 1) current_page++;
        if (ch == 'p' && current_page > 0) current_page--;


    } while (ch != 'q');

    clear();
    refresh();
}

// Function to read scores for scoreboard
int read_scores(const char *filename, Player players[], int max_players) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        mvprintw(1, 1, "Error: Unable to open scores file.");
        refresh();
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%s %d %d %d %s", 
                  players[count].username, 
                  &players[count].total_points, 
                  &players[count].gold_points, 
                  &players[count].games_completed, 
                  players[count].first_game_time) != EOF && count < max_players) {
        count++;
    }

    fclose(file);
    return count;
}

// Function to display options menu
void display_options() {
    char *color_preview[] = {
        "⚪ White ░░░░░░░░░░░░░░░░░░░",
        "🔴 Red █████████████████████"
    };
    
    char *music_icons[] = {
        "🔇 Music OFF", 
        "🎵 Music ON  ♫♫♫"
    };
    
    char *difficulty_slider[] = {
        "[☐─────────]", // Easy
        "[───☐──────]", // Medium
        "[───────☐──]"  // Hard
    };;

    int highlight = 0;
    int choice = 0;
    int width = 70;
    int height = 14;
    int startx = (COLS - width) / 2;
    int starty = (LINES - height) / 2;

    WINDOW *settings_win = newwin(height, width, starty, startx);
    wattron(settings_win, COLOR_PAIR(5) | A_BLINK);
        wborder(settings_win, 
            '|', '|',
            '=', '=',
            '+', '+',
            '+', '+'
        );
        wattroff(settings_win, COLOR_PAIR(5) | A_BLINK);
    keypad(settings_win, TRUE);


    while (1) {
        //clear();
        //box(settings_win, 0, 0);

        wattron(settings_win, COLOR_PAIR((character_color == 1) ? 0 : 3));
        mvwprintw(settings_win, 1, (width / 2) - 1, "X");
        wattroff(settings_win, COLOR_PAIR((character_color == 1) ? 0 : 3));

        mvwprintw(settings_win, 3, (width - 15) / 2, "Settings Menu");
        mvwprintw(settings_win, 5, 2, "1. Change Difficulty: %s", difficulty_slider[game_difficulty - 1]);
        mvwprintw(settings_win, 6, 2, "2. Change Character Color: %s", color_preview[character_color - 1]);
        mvwprintw(settings_win, 7, 2, music_enabled ? "3. Music: 🎵 Music ON  ♫♫♫" : "3. Music: 🔇 Music OFF");
        mvwprintw(settings_win, 8, 2, "4. Default Music: %d", default_music);
        mvwprintw(settings_win, 9, 2, "5. Back to Main Menu");

        for (int i = 0; i < 5; i++) {
            if (highlight == i) {
                wattron(settings_win, A_REVERSE);
                mvwprintw(settings_win, 5 + i, 2, "%d. %s", i + 1,
                          i == 0 ? "Change Difficulty" :
                          i == 1 ? "Change Character Color" :
                          i == 2 ? "Music" :
                          i == 3 ? "Default Music" :
                          "Back to Main Menu");
                wattroff(settings_win, A_REVERSE);
            }
        }
        wrefresh(settings_win);

        int ch = wgetch(settings_win);
        if (ch == KEY_UP) {
            highlight = (highlight - 1 + 5) % 5;
        } else if (ch == KEY_DOWN) {
            highlight = (highlight + 1) % 5;
        } else if (ch == 10) {
            choice = highlight + 1;
            if (choice == 1) {
                game_difficulty = (game_difficulty % 3) + 1;
                mvwprintw(settings_win, 5, 25, "                   ");
                mvwprintw(settings_win, 5, 25, "%s", difficulty_slider[game_difficulty - 1]);
            } else if (choice == 2) {
                character_color = (character_color % 2) + 1;
                mvwprintw(settings_win, 6, 25, "                 ");
                mvwprintw(settings_win, 6, 25, "%s", color_preview[character_color - 1]);
                wattron(settings_win, COLOR_PAIR((character_color == 1) ? 0 : 3));
                mvwprintw(settings_win, 1, (width / 2) - 1, "X");
                wattroff(settings_win, COLOR_PAIR((character_color == 1) ? 0 : 3));
            } else if (choice == 3) {
                mvwprintw(settings_win, 7, 25, "                 ");
                music_enabled = !music_enabled;
            } else if (choice == 4) {
                default_music = (default_music % 4) + 1;
            } else if (choice == 5) {
                break;
            }
        } else if (ch == 'q') break;
    }

    clear();
    refresh();
    delwin(settings_win);
}
void profile_menu() {
    clear();

    char *frame[] = {
        "╔══════════════•◦ 📊 STATS ◦•══════════════╗",
        "║                                          ║",
        "╚══════════════════════════════════════════╝"
    };
    mvprintw(19, (COLS - 50) / 2, frame[0]);
    mvprintw(20, (COLS - 50) / 2, frame[1]);
    mvprintw(21, (COLS - 50) / 2, frame[1]);
    mvprintw(22, (COLS - 50) / 2, frame[1]);
    mvprintw(23, (COLS - 50) / 2, frame[1]);
    mvprintw(24, (COLS - 50) / 2, frame[1]);
    mvprintw(25, (COLS - 50) / 2, frame[2]);
    
    mvprintw(20, (COLS - 20) / 2, "👨‍🚀Profile");
    mvprintw(21, (COLS - 20) / 2, "%s", player_username);

    Player players[MAX_PLAYERS];
    int n_players = read_scores("scores.txt", players, MAX_PLAYERS);
    for (int i = 0; i < n_players; i++){
        if (strcmp(players[i].username, player_username) == 0) {
            mvprintw(22, (COLS - 20) / 2, "Total Scores: %d", players[i].total_points);
            mvprintw(23, (COLS - 20) / 2, "Total Golds: %d", players[i].gold_points);
            mvprintw(24, (COLS - 20) / 2, "Games Completed: %d", players[i].games_completed);
        }
    }

    getch();
    clear();
    refresh();
}