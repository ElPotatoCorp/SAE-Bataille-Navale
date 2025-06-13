#include "battleship.h"

int SOCKET_FD;
int PLAYER;
const char *IP_ADDRESS;
bool HOST_MODE;
bool DEBUG;

char COPY_GRID_ENEMY[DIM][DIM];
int LAST_X, LAST_Y;
int LAST_OPPONENT_X, LAST_OPPONENT_Y;
const char *SHOT_MSG;
bool END;

void clear() {  
#ifdef _WIN32
    system("cls");
#elif __linux__
    system("clear");
#endif
}

void game_pause() {
    printf("Press enter to continue...");
    while (getchar() != '\n');
}

void initialize_grid(char grid[DIM][DIM]) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            grid[i][j] = '-';
        }
    }
}

void display_box(char c, bool highlight) {
    if (highlight) {
        printf("\033[38;2;255;255;0m%c \033[0m", c); // Highlighted case
        return;
    } 

    if (END) 
    {
        switch (c) {
        case 'X':
            printf("\033[38;2;255;0;0m%c \033[0m", c); // Red for hit
            break;
        case 'O':
            printf("%c ", c); // Normal case
            break;
        case '#': // Carrier (size 5) - darkest red
            printf("\033[38;2;120;0;0m%c \033[0m", c);
            break;
        case '@': // Battleship (size 4)
            printf("\033[38;2;170;0;0m%c \033[0m", c);
            break;
        case '%': // Cruiser (size 3)
            printf("\033[38;2;210;0;0m%c \033[0m", c);
            break;
        case '&': // Submarine (size 3)
            printf("\033[38;2;230;40;40m%c \033[0m", c);
            break;
        case '$': // Destroyer (size 2) - brightest red
            printf("\033[38;2;255;80;80m%c \033[0m", c);
            break;
        default:
            printf("\033[38;2;0;150;255m%c \033[0m", c); // Default color for empty cells
            break;
        }
		return;
    }

    switch (c) {
        case 'X':
            printf("\033[38;2;255;0;0m%c \033[0m", c); // Red for hit
            break;
        case 'O':
            printf("%c ", c); // Normal case
            break;
        case '#': // Carrier (size 5) - darkest green
            printf("\033[38;2;0;80;0m%c \033[0m", c);
            break;
        case '@': // Battleship (size 4)
            printf("\033[38;2;0;120;0m%c \033[0m", c);
            break;
        case '%': // Cruiser (size 3)
            printf("\033[38;2;0;170;0m%c \033[0m", c);
            break;
        case '&': // Submarine (size 3)
            printf("\033[38;2;0;200;0m%c \033[0m", c);
            break;
        case '$': // Destroyer (size 2) - brightest green
            printf("\033[38;2;0;255;0m%c \033[0m", c);
            break;
        default:
            printf("\033[38;2;0;150;255m%c \033[0m", c); // Default color for empty cells
            break;
    }
}

void display_grid(char grid[DIM][DIM], int x, int y) {
    printf("  1 2 3 4 5 6 7 8 9 0");
    for (int i = 0; i < DIM; i++) {
        printf("\n%c ", 'A' + i);
        for (int j = 0; j < DIM; j++) {
            display_box(grid[i][j], (x == i && y == j));
        }
    }
    printf("\n");
}

void display_board(char fleet_grid[DIM][DIM], char shot_grid[DIM][DIM], int x, int y, int x_bis, int y_bis) {
    display_grid(fleet_grid, x, y);
    printf("---------------------\n");
    display_grid(shot_grid, x_bis, y_bis);
}

void display_final_board(char shot[DIM][DIM], char shot_enemy[DIM][DIM]) {
    printf("  1 2 3 4 5 6 7 8 9 0");
    for (int i = 0; i < DIM; i++) {
        printf("\n%c ", 'A' + i);
        for (int j = 0; j < DIM; j++) {
            END = shot[i][j] == 'X';
            display_box(COPY_GRID_ENEMY[i][j], false);
        }
    }
    printf("\n---------------------\n");
    display_grid(shot_enemy, -1, -1);
    END = true;
}

void grid_to_string(char grid[DIM][DIM], char *buffer, size_t bufsize) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            buffer[i * DIM + j] = grid[i][j];
        }
    }
    buffer[bufsize - 1] = '\0';
}

void string_to_grid(const char *buffer, char grid[DIM][DIM]) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            grid[i][j] = buffer[i * DIM + j];
        }
    }
}

void get_coord(int coord[2]) {
    char line[128];
    char letter = 0;
    int number = -1;

    while (1) {
        printf("Enter coordinates (Letter A-J and Number 0-9, e.g. B 7): ");
        if (!fgets(line, sizeof(line), stdin))
            continue;

#ifdef _WIN32
        if (!(sscanf_s(line, " %c %d", &letter, (unsigned int)sizeof(letter), &number) == 2 || sscanf_s(line, " %c%d", &letter, (unsigned int)sizeof(letter), &number) == 2)) {
            printf("Invalid input. Please enter a letter (A-J) and a number (0-9).\n");
        }
#elif __linux__
        if (!(sscanf(line, " %c %d", &letter, &number) == 2 || sscanf(line, " %c%d", &letter, &number) == 2)) {
            printf("Invalid input. Please enter a letter (A-J) and a number (0-9).\n");
        }
#endif
        else {
            letter = toupper(letter);
            if (letter >= 'A' && letter <= 'J') {
                coord[0] = letter - 'A';
            } else {
                printf("Invalid letter. Please enter a letter from A to J.\n");
                continue;
            }

            if (number == 0) {
                coord[1] = 9;
            } else if (number >= 1 && number <= 9) {
                coord[1] = number - 1;
            } else {
                printf("Invalid number. Please enter a number from 0 to 9.\n");
                continue;
            }
            break;
        }
    }
}

char *coord_to_string(int *coord) {  
    if (coord == NULL) {  
        fprintf(stderr, "Error: NULL pointer passed to coord_to_string.\n");  
        return NULL;  
    }  

    char *coord_str = malloc(3 * sizeof(char));  
    if (coord_str == NULL) {  
        fprintf(stderr, "Error: Memory allocation failed in coord_to_string.\n");  
        return NULL;  
    }  

    coord_str[0] = '0' + coord[0];  
    coord_str[1] = '0' + coord[1];  
    coord_str[2] = '\0';  

    return coord_str;  
}

bool can_be_placed(int size, int rot, int i, int j, char grid[DIM][DIM]) {
    for (int k = 0; k < size; ++k) {
        int x = i + (rot == 3 ? k : rot == 1 ? -k : 0);
        int y = j + (rot == 2 ? k : rot == 4 ? -k : 0);
        if (grid[x][y] != '-') return false;
    }
    return true;
}

void place_ship(int size, int rot, int i, int j, char symbol, char grid[DIM][DIM]) {
    for (int k = 0; k < size; ++k) {
        int x = i + (rot == 3 ? k : rot == 1 ? -k : 0);
        int y = j + (rot == 2 ? k : rot == 4 ? -k : 0);
        grid[x][y] = symbol;
    }
}

void placement(char grid[DIM][DIM], int player, Ship fleet[]) {
    printf("Placement of Player %d's ships\n", player);

    while (true) {
        display_grid(grid, -1, -1);

        // Print available ships
        printf("Choose a ship (1-5):\n");
        for (int i = 0; i < 5; i++) {
            if (fleet[i].active)
                printf("%d. %s - size: %d - symbol: '%c'\n", i + 1, fleet[i].name, fleet[i].size, fleet[i].symbol);
        }
        printf("Format: <ship> <letter> <number> <rotation>\n");
        printf("Example: 2 B 7 1\n");

        int choice = -1, x = -1, y = -1, rot = -1;
        char letter = 0;
        char line[128];

        // Prompt for all values at once
        printf("Enter placement: ");
        if (!fgets(line, sizeof(line), stdin)) continue;

        // Try to parse all values at once (case-insensitive for letter)
#ifdef _WIN32
		if (sscanf_s(line, "%d %c %d %d", &choice, &letter, (unsigned int)sizeof(letter), &y, &rot) == 4) {
            if (choice < 1 || choice > 5 || !fleet[choice - 1].active) {
                clear();
                printf("Invalid ship number. Try again.\n");
                continue;
            }
            letter = toupper(letter);
            if (letter < 'A' || letter > 'J') {
                clear();
                printf("Invalid letter. Try again.\n");
                continue;
            }
            if (y < 0 || y > 9) {
                clear();
                printf("Invalid number. Try again.\n");
                continue;
            }
            if (rot < 1 || rot > 4) {
                clear();
                printf("Invalid rotation. Try again.\n");
                continue;
            }

            choice -= 1;
            x = letter - 'A';
            if (y == 0) y = 9; else y -= 1;
        }
#elif __linux__
        if (sscanf(line, "%d %c %d %d", &choice, &letter, &y, &rot) == 4) {
            choice -= 1;
            x = toupper(letter) - 'A';
            if (y == 0) y = 9; else y -= 1;
        }
#endif
        else {
            // Fallback: ask for each value individually if missing or invalid

            // Reset values
            choice = -1, x = -1, y = -1, rot = -1;

            // Ship number
            while (choice < 0 || choice >= 5 || !fleet[choice].active) {
                printf("Ship number [1-5]: ");
                if (!fgets(line, sizeof(line), stdin)) continue;
#ifdef _WIN32
				if (sscanf_s(line, "%d", &choice) == 1) {
                    choice -= 1;
                }
#elif __linux__
                if (sscanf(line, "%d", &choice) == 1) {
                    choice -= 1;
                }
#endif
                else if (isdigit(line[0])) {
                    choice = line[0] - '1';
                }
                else {
                    continue;
                }
                if (choice < 0 || choice >= 5 || !fleet[choice].active) printf("Invalid ship. Try again.\n");
            }

            // Letter
            while (x < 0 || x >= DIM) {
                printf("Letter (A-J): ");
                if (!fgets(line, sizeof(line), stdin)) continue;
#ifdef _WIN32
                if (sscanf_s(line, " %c", &letter, (unsigned int)sizeof(letter)) == 1 && isalpha(letter)) {
                    x = toupper(letter) - 'A';
                }
#elif __linux__
				if (sscanf(line, " %c", &letter) == 1 && isalpha(letter)) {
                    x = toupper(letter) - 'A';
                }
#endif
                if (x < 0 || x >= DIM) printf("Invalid letter. Try again.\n");
            }

            // Number
            while (y < 0 || y >= DIM) {
                printf("Number (0-9): ");
                if (!fgets(line, sizeof(line), stdin)) continue;
#ifdef _WIN32
                if (sscanf_s(line, "%d", &y) != 1) {
                    if (isdigit(line[0])) y = line[0] - '0';
                    else continue;
                }
#elif __linux__
				if (sscanf(line, "%d", &y) != 1) {
                    if (isdigit(line[0])) y = line[0] - '0';
                    else continue;
                }
#endif
                if (y == 0) y = 9; else y -= 1;
                if (y < 0 || y >= DIM) printf("Invalid number. Try again.\n");
            }

            // Rotation
            while (rot < 1 || rot > 4) {
                printf("Rotation (1=north, 2=east, 3=south, 4=west): ");
                if (!fgets(line, sizeof(line), stdin)) continue;
#ifdef _WIN32
                sscanf_s(line, "%d", &rot);
#elif __linux__
                sscanf(line, "%d", &rot);
#endif
                if (rot < 1 || rot > 4) printf("Invalid rotation. Try again.\n");
            }
        }

        if (!can_be_placed(fleet[choice].size, rot, x, y, grid)) {
            printf("Invalid placement.\n");
            continue;
        }

        place_ship(fleet[choice].size, rot, x, y, fleet[choice].symbol, grid);
        fleet[choice].active = false;
        if (!DEBUG) clear();

        // Check if all ships placed
        bool finished = true;
        for (int k = 0; k < 5; k++) {
            if (fleet[k].active) finished = false;
        }
        if (finished) break;
    }
}

void shoot(char enemy_grid[DIM][DIM], char shots_grid[DIM][DIM], int* ship_health, int x, int y, bool mute) {
    if (DEBUG) printf("Shooting at (%d, %d)\n", x, y);

    if (shots_grid[x][y] != '-') {
        SHOT_MSG = !mute ? "Already shot here." : NULL;
        return;
    }

    if (enemy_grid[x][y] != '-') {
        char symbol = enemy_grid[x][y];
        shots_grid[x][y] = 'X';
        enemy_grid[x][y] = 'X';
        ship_health[symbol]--;
        SHOT_MSG = !mute ? (ship_health[symbol] == 0 ? "Sunk!" : "Hit!") : NULL;
        return;
    }
    else {
        enemy_grid[x][y] = 'O';
        shots_grid[x][y] = 'O';
        SHOT_MSG = !mute ? "Miss..." : NULL;
        return;
    }
}

void server_communication_handler(int socket, char* buffer, int bufsize, const char *message)
{
    int code = receive_message(SOCKET_FD, buffer, MSG_LEN - 1, DEBUG);
    if (code == 404) {
        fprintf(stdout, "Warning: %s\n", "The other player has left or the connexion has been closed.");
		close_connection(socket, DEBUG);
		exit(EXIT_SUCCESS);
    }
	if (code <= 0) {
        fprintf(stderr, "Error: %s\n", message);
        close_connection(socket, DEBUG);
        exit(EXIT_FAILURE);
	}
}

void send_info_to_opponent(char *info) {
    if (HOST_MODE) send_server_message(SOCKET_FD, info, DEBUG);
    else send_message(SOCKET_FD, info, DEBUG);
}

void receive_info_from_opponent(char *buffer, int bufsize, const char *error_message) {
    if (HOST_MODE) receive_client_message(SOCKET_FD, buffer, bufsize, DEBUG);
	else server_communication_handler(SOCKET_FD, buffer, bufsize, error_message);
}

void placement_screen(char grid[DIM][DIM], Ship fleet[5], char enemy_grid[DIM][DIM]) {
    char grid_str[DIM * DIM + 1];
    placement(grid, PLAYER, fleet);
    grid_to_string(grid, grid_str, sizeof(grid_str));

    if (!DEBUG) clear();
    printf("Waiting for other player...\n");
    fflush(stdout);

    send_info_to_opponent(grid_str);
    receive_info_from_opponent(grid_str, DIM * DIM + 1, "Error receiving grid.");
    string_to_grid(grid_str, enemy_grid);
    memcpy(COPY_GRID_ENEMY, enemy_grid, sizeof(COPY_GRID_ENEMY));
    if (DEBUG) game_pause();
}

void game_over_screen(char shots[DIM][DIM], char shots_enemy[DIM][DIM], char *message, int player) {
    printf("\n--- Game Over : %s ---\n\n", message);
    printf("Enemy's board:\n");
    display_final_board(shots, shots_enemy);
    printf("Player %d won!\n", player);
}

void action_screen(char grid[DIM][DIM], char shots[DIM][DIM], char grid_enemy[DIM][DIM], char shots_enemy[DIM][DIM], int *health) {
    printf("Your board:\n");
    display_board(grid, shots, LAST_OPPONENT_X, LAST_OPPONENT_Y, LAST_X, LAST_Y);

    int coord[2] = { 0 };
    get_coord(coord);

    LAST_X = coord[0];
    LAST_Y = coord[1];
    shoot(grid_enemy, shots, health, LAST_X, LAST_Y, false);
    
    if (health['#'] == 0 && health['@'] == 0 && health['%'] == 0 && health['&'] == 0 && health['$'] == 0) {
        char state[MSG_LEN] = { 0 };
        snprintf(state, sizeof(state), "END%d%d", LAST_X, LAST_Y);
        END = true;

        game_over_screen(shots, shots_enemy, "You won !", PLAYER);
        send_info_to_opponent(state);
    } else {
        send_info_to_opponent(coord_to_string(coord));
    }
}

void waiting_screen(char grid[DIM][DIM], char shots[DIM][DIM], char grid_enemy[DIM][DIM], char shots_enemy[DIM][DIM], int *health) {
    printf("Your board:\n");
    display_board(grid, shots, LAST_OPPONENT_X, LAST_OPPONENT_Y, LAST_X, LAST_Y);
    printf("%s\n", SHOT_MSG);

    char state[MSG_LEN] = { 0 };
    receive_info_from_opponent(state, MSG_LEN,"Error receiving state.");

    if (strncmp(state, "END", 3) == 0) {
        shoot(grid, shots_enemy, health, state[3] - '0', state[4] - '0', true);
        END = true;

        game_over_screen(shots, shots_enemy, "You lost...", PLAYER == 1 ? 2 : 1);
        return;
    }

	LAST_OPPONENT_X = state[0] - '0';
	LAST_OPPONENT_Y = state[1] - '0';
    shoot(grid, shots_enemy, health, LAST_OPPONENT_X, LAST_OPPONENT_Y, true);

    if (DEBUG) printf("State: %s\n", state);
}

void play(const char* ip_address, bool restarted, bool host_mode, bool debug) {
    LAST_X = -1;
    LAST_Y = -1;
    LAST_OPPONENT_X = -1;
    LAST_OPPONENT_Y = -1;
    SHOT_MSG = NULL;
    END = false;

    int turn = 0;
	HOST_MODE = host_mode;
    DEBUG = debug;
    if (!restarted && !HOST_MODE) {
        printf("Connecting to %s...\n", ip_address);
        SOCKET_FD = connection_loop(ip_address, debug);
        if (SOCKET_FD != INVALID_SOCKET) {
			printf("Connected to %s\n", ip_address);
            printf("Waiting for other player...\n");
            char recv_buffer[MSG_LEN] = { 0 };
            receive_info_from_opponent(recv_buffer, 3, "Error connecting to server.");
            PLAYER = recv_buffer[0] - '0';
            turn = recv_buffer[1] - '0';
        }
        IP_ADDRESS = ip_address;
    }
    else if (HOST_MODE) {
        PLAYER = 1;
#ifdef _WIN32
		sscanf_s(ip_address, "%d %d", &turn, &SOCKET_FD);
#elif __linux__
		sscanf(ip_address, "%d %d", &turn, &SOCKET_FD);
#endif
    }

    char grid_P1[DIM][DIM], grid_P2[DIM][DIM];
    char shots_P1[DIM][DIM], shots_P2[DIM][DIM];

    if (PLAYER == 1) initialize_grid(grid_P1);
    else initialize_grid(grid_P2);
    initialize_grid(shots_P1);
    initialize_grid(shots_P2);

    Ship fleet_P1[5] = { {"Carrier",'#',5,5,true},{"Battleship",'@',4,4,true},{"Cruiser",'%',3,3,true},{"Submarine",'&',3,3,true},{"Destroyer",'$',2,2,true} };
    Ship fleet_P2[5];
    memcpy(fleet_P2, fleet_P1, sizeof(fleet_P1));

    int health_P1[128] = { 0 }, health_P2[128] = { 0 };
    health_P1['#'] = 5; health_P1['@'] = 4; health_P1['%'] = 3; health_P1['&'] = 3; health_P1['$'] = 2;
    memcpy(health_P2, health_P1, sizeof(health_P1));

    if (PLAYER == 1) {
        placement_screen(grid_P1, fleet_P1, grid_P2);
    }
    else {
        placement_screen(grid_P2, fleet_P2, grid_P1);
    }

    
    while (!END) {
        if (!DEBUG) clear();
        printf("Player %d's turn\n", turn);
        if (turn == 1) {
            if (PLAYER == 1) {
                action_screen(grid_P1, shots_P1, grid_P2, shots_P2, health_P2);
            } else {
                waiting_screen(grid_P2, shots_P2, grid_P1, shots_P1, health_P2);
            }
        } else {
            if (PLAYER == 2) {
                action_screen(grid_P2, shots_P2, grid_P1, shots_P1, health_P1);
            } else {
                waiting_screen(grid_P1, shots_P1, grid_P2, shots_P2, health_P1);
            }
        }
        turn = (turn == 1) ? 2 : 1;
    }
}
