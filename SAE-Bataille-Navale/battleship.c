#include "battleship.h"

int SOCKET_FD;
int PLAYER;
const char *IP_ADDRESS;
bool DEBUG;
int LAST_X = -1, LAST_Y = -1;

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

void display_grid(char grid[DIM][DIM]) {
    printf("  1 2 3 4 5 6 7 8 9 0");
    for (int i = 0; i < DIM; i++) {
        printf("\n%c ", 'A' + i);
        for (int j = 0; j < DIM; j++) {
            if (i == LAST_X && j == LAST_Y) {
                // Displays the last shot in color with the Ansi Code
                printf("\033[1;33m%c \033[0m", grid[i][j]);
            }
            else {
                switch (grid[i][j]) {
					case '-':
						printf("%c ", grid[i][j]);
						break;
					case 'X':
						printf("\033[1;31m%c \033[0m", grid[i][j]);
						break;
					case 'O':
						printf("\033[1;34m%c \033[0m", grid[i][j]);
                        break;
					default:
						printf("\033[1;32m%c \033[0m", grid[i][j]);
						break;
				}
            }
        }
    }
    printf("\n");
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

int *get_coord(void) {
    int *coord = malloc(2 * sizeof(int));
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

    return coord;
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

bool is_valid(int i, int j) {
    return i >= 0 && i < DIM && j >= 0 && j < DIM;
}

bool can_be_placed(int size, int rot, int i, int j, char grid[DIM][DIM]) {
    for (int k = 0; k < size; ++k) {
        int x = i + (rot == 3 ? k : rot == 1 ? -k : 0);
        int y = j + (rot == 2 ? k : rot == 4 ? -k : 0);
        if (!is_valid(x, y) || grid[x][y] != '-') return false;
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
        display_grid(grid);

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
            choice -= 1;
            x = toupper(letter) - 'A';
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
        clear();

        // Check if all ships placed
        bool finished = true;
        for (int k = 0; k < 5; k++) {
            if (fleet[k].active) finished = false;
        }
        if (finished) break;
    }
}

bool shoot(char enemy_grid[DIM][DIM], char shots_grid[DIM][DIM], int* ship_health, int x, int y, bool mute) {
    if (DEBUG) printf("Shooting at (%d, %d)\n", x, y);

    if (shots_grid[x][y] != '-') {
        if (!mute) printf("You already fired here.\n");
        return false;
    }

    if (enemy_grid[x][y] != '-') {
        if (!mute) printf("Hit!\n");
        char symbol = enemy_grid[x][y];
        shots_grid[x][y] = 'X';
        enemy_grid[x][y] = 'X';
        ship_health[symbol]--;
        if (ship_health[symbol] == 0) {
            if (!mute) printf("Ship sunk!\n");
        }
        return true;
    }
    else {
        if (!mute) printf("Miss.\n");
        enemy_grid[x][y] = 'O';
        shots_grid[x][y] = 'O';
        return false;
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

void placement_screen(char grid[DIM][DIM], Ship fleet[5], char enemy_grid[DIM][DIM]) {
    char grid_str[DIM * DIM + 1];
    placement(grid, PLAYER, fleet);
    grid_to_string(grid, grid_str, sizeof(grid_str));

    clear();
    printf("Waiting for other player...\n");
    fflush(stdout);

    send_message(SOCKET_FD, grid_str, DEBUG);
    server_communication_handler(SOCKET_FD, grid_str, DIM * DIM + 1, "Error receiving grid.");
    string_to_grid(grid_str, enemy_grid);
    if (DEBUG) game_pause();
}

void action_screen(char grid[DIM][DIM], char shots[DIM][DIM], int *health, bool *end) {
    printf("Your shots grid:\n");
    display_grid(shots);

    int *coord = get_coord();
    if (!is_valid(coord[0], coord[1])) {
        printf("Invalid coordinates.\n");
        game_pause();

        send_message(SOCKET_FD, "INVALID", DEBUG);

        free(coord);
        return;
    }
    else {
        shoot(grid, shots, health, coord[0], coord[1], false);
    }
    
    if (health['#'] == 0 && health['@'] == 0 && health['%'] == 0 && health['&'] == 0 && health['$'] == 0) {
        printf("Player %d won!\n", PLAYER);
        *end = true;
        send_message(SOCKET_FD, "END", DEBUG);
    } else {
        game_pause();
        send_message(SOCKET_FD, coord_to_string(coord), DEBUG);
    }

    free(coord);
}

void waiting_screen(char grid[DIM][DIM], char grid_enemy[DIM][DIM], char shots_enemy[DIM][DIM], int *health, bool *end) {
    printf("Your fleet grid:\n");
    display_grid(grid);

    char state[MSG_LEN] = { 0 };
	server_communication_handler(SOCKET_FD, state, MSG_LEN - 1,"Error receiving state.");

    if (strcmp(state, "END") == 0) {
        clear();
		printf("Enemy fleet grid:\n");
        display_grid(grid_enemy);
        printf("Player %d won!\n", PLAYER);
        *end = true;
    } else if (strcmp(state, "INVALID") != 0) {
        LAST_X = state[0] - '0';
		LAST_Y = state[1] - '0';
        shoot(grid, shots_enemy, health, LAST_X, LAST_Y, !DEBUG);
        clear();
        printf("Your fleet grid:\n");
        display_grid(grid);
    }

    if (DEBUG) printf("State: %s\n", state);
    if (!*end) game_pause();
}

void play(const char* ip_address, bool debug) {
    printf("Connecting to %s...\n", ip_address);
    SOCKET_FD = connection_loop(ip_address, debug);
    int turn;
	if (SOCKET_FD != INVALID_SOCKET) {
		char recv_buffer[MSG_LEN] = { 0 };
		server_communication_handler(SOCKET_FD, recv_buffer, 3, "Error connecting to server.");
        PLAYER = recv_buffer[0] - '0';
        turn = recv_buffer[1] - '0';
	}
    IP_ADDRESS = ip_address;
    DEBUG = debug;

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

    bool end = false;
    while (!end) {
        clear();
        printf("Player %d's turn\n", turn);
        if (turn == 1) {
            if (PLAYER == 1) {
                action_screen(grid_P2, shots_P1, health_P2, &end);
            } else {
                waiting_screen(grid_P2, grid_P1, shots_P1, health_P2, &end);
            }
        } else {
            if (PLAYER == 2) {
                action_screen(grid_P1, shots_P2, health_P1, &end);
            } else {
                waiting_screen(grid_P1, grid_P2, shots_P2, health_P1, &end);
            }
        }
        turn = (turn == 1) ? 2 : 1;
    }

    close_connection(SOCKET_FD, DEBUG);
}
