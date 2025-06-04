#include "battleship.h"

const char *IP_ADDRESS;
bool DEBUG;

void game_pause()
{
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

void display_grid(char grid[DIM][DIM], bool refresh) {
    if (refresh)
        printf("\033[%dA", DIM + 2); // Move up (number of lines grid uses)
    printf("  1 2 3 4 5 6 7 8 9 0");
    for (int i = 0; i < DIM; i++) {
        printf("\n%c ", 'A' + i);
        for (int j = 0; j < DIM; j++) {
            printf("%c ", grid[i][j]);
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

        if (sscanf(line, " %c %d", &letter, &number) == 2 ||
            sscanf(line, " %c%d", &letter, &number) == 2) {
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
        } else {
            printf("Invalid input. Please enter a letter (A-J) and a number (0-9).\n");
        }
    }

    return coord;
}

char *coord_to_string(int *coord) {
    char *coord_str = malloc(3 * sizeof(char));
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

    while (1) {
        display_grid(grid, false);

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
        if (sscanf(line, "%d %c %d %d", &choice, &letter, &y, &rot) == 4) {
            choice -= 1;
            x = toupper(letter) - 'A';
            if (y == 0) y = 9; else y -= 1;
        } else {
            // Fallback: ask for each value individually if missing or invalid

            // Reset values
            choice = -1, x = -1, y = -1, rot = -1;

            // Ship number
            while (choice < 0 || choice >= 5 || !fleet[choice].active) {
                printf("Ship number [1-5]: ");
                if (!fgets(line, sizeof(line), stdin)) continue;
                if (sscanf(line, "%d", &choice) == 1) {
                    choice -= 1;
                } else if (isdigit(line[0])) {
                    choice = line[0] - '1';
                } else {
                    continue;
                }
                if (choice < 0 || choice >= 5 || !fleet[choice].active) printf("Invalid ship. Try again.\n");
            }

            // Letter
            while (x < 0 || x >= DIM) {
                printf("Letter (A-J): ");
                if (!fgets(line, sizeof(line), stdin)) continue;
                if (sscanf(line, " %c", &letter) == 1 && isalpha(letter)) {
                    x = toupper(letter) - 'A';
                }
                if (x < 0 || x >= DIM) printf("Invalid letter. Try again.\n");
            }

            // Number
            while (y < 0 || y >= DIM) {
                printf("Number (0-9): ");
                if (!fgets(line, sizeof(line), stdin)) continue;
                if (sscanf(line, "%d", &y) != 1) {
                    if (isdigit(line[0])) y = line[0] - '0';
                    else continue;
                }
                if (y == 0) y = 9; else y -= 1;
                if (y < 0 || y >= DIM) printf("Invalid number. Try again.\n");
            }

            // Rotation
            while (rot < 1 || rot > 4) {
                printf("Rotation (1=north, 2=east, 3=south, 4=west): ");
                if (!fgets(line, sizeof(line), stdin)) continue;
                sscanf(line, "%d", &rot);
                if (rot < 1 || rot > 4) printf("Invalid rotation. Try again.\n");
            }
        }

        if (!can_be_placed(fleet[choice].size, rot, x, y, grid)) {
            printf("Invalid placement.\n");
            continue;
        }

        place_ship(fleet[choice].size, rot, x, y, fleet[choice].symbol, grid);
        fleet[choice].active = false;
        system("clear");

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
        if (ship_health[symbol] == 0)
        {
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

void game_error(const char *message)
{
    printf("%s\n", message);
    exit(0);
}

void action_screen(char grid[DIM][DIM], char shots[DIM][DIM], int *health, bool *end) {
    printf("Your shots grid:\n");
    display_grid(shots, false);

    int *coord = get_coord();
    if (!is_valid(coord[0], coord[1])) {
        printf("Invalid coordinates.\n");
        game_pause();

        if (!try_send_infos(IP_ADDRESS, "INVALID", DEBUG)) game_error("Other player disconnected. Game interrupted.");

        free(coord);
        return;
    }
    else {
        shoot(grid, shots, health, coord[0], coord[1], false);
    }
    
    if (health['#'] == 0 && health['@'] == 0 && health['%'] == 0 && health['&'] == 0 && health['$'] == 0) {
        printf("Player 1 won!\n");
        *end = true;
        if (!try_send_infos(IP_ADDRESS, "END", DEBUG)) game_error("Other player disconnected. Game interrupted.");
    } else {
        game_pause();
        if (!try_send_infos(IP_ADDRESS, coord_to_string(coord), DEBUG)) game_error("Other player disconnected. Game interrupted.");
    }

    free(coord);
}

void waiting_screen(char grid[DIM][DIM], char grid_enemy[DIM][DIM], char shots_enemy[DIM][DIM], int *health, bool *end) {
    printf("Your fleet grid:\n");
    display_grid(grid, false);

    const char *state = recv_infos();

    if (strcmp(state, "END") == 0) {
        display_grid(grid_enemy, false);
        printf("Player 1 won!\n");
        *end = true;
    } else if (strcmp(state, "INVALID") != 0){
        shoot(grid, shots_enemy, health, state[0] - '0', state[1] - '0', !DEBUG);
        display_grid(grid, !DEBUG);
    }

    if (DEBUG) printf("State: %s\n", state);
    game_pause();
}

void play(int player, const char* ip_address, bool debug) {
    IP_ADDRESS = ip_address;
    DEBUG = debug;

    char grid_P1[DIM][DIM], grid_P2[DIM][DIM];
    char shots_P1[DIM][DIM], shots_P2[DIM][DIM];

    if (player == 1) initialize_grid(grid_P1);
    else initialize_grid(grid_P2);
    initialize_grid(shots_P1);
    initialize_grid(shots_P2);

    Ship fleet_P1[5] = { {"Carrier",'#',5,5,true},{"Battleship",'@',4,4,true},{"Cruiser",'%',3,3,true},{"Submarine",'&',3,3,true},{"Destroyer",'$',2,2,true} };
    Ship fleet_P2[5];
    memcpy(fleet_P2, fleet_P1, sizeof(fleet_P1));

    int health_P1[128] = { 0 }, health_P2[128] = { 0 };
    health_P1['#'] = 5; health_P1['@'] = 4; health_P1['%'] = 3; health_P1['&'] = 3; health_P1['$'] = 2;
    memcpy(health_P2, health_P1, sizeof(health_P1));

    int turn;
    if (player == 1) {
        turn = rand() % 2 + 1;

        char turn_str = turn + '0';

        printf("Waiting for other player to connect...\n");
        if (!try_send_infos(IP_ADDRESS, &turn_str, DEBUG)) game_error("Other player disconnected. Game interrupted.");
        printf("Turn: %d\n", turn);
    }
    else {
        printf("Waiting for other player to connect...\n");
        turn = atoi(recv_infos());
        printf("Turn: %d\n", turn);
    }

    char grid_str[DIM * DIM + 1];
    if (player == 1) {
        placement(grid_P1, 1, fleet_P1);
        grid_to_string(grid_P1, grid_str, sizeof(grid_str));

        system("clear");
        printf("Waiting for other player...\n");

        if (!try_send_infos(IP_ADDRESS, grid_str, DEBUG)) game_error("Other player disconnected. Game interrupted.");
        string_to_grid(recv_infos(), grid_P2);
        if (DEBUG) game_pause();
    }
    else {
        placement(grid_P2, 2, fleet_P2);
        grid_to_string(grid_P2, grid_str, sizeof(grid_str));

        system("clear");
        printf("Waiting for other player...\n");

        string_to_grid(recv_infos(), grid_P1);
        if (!try_send_infos(IP_ADDRESS, grid_str, DEBUG)) game_error("Other player disconnected. Game interrupted.");
        if (DEBUG) game_pause();
    }

    bool end = false;
    while (!end)
    {
        system("clear");
        printf("Player %d's turn\n", turn);
        if (turn == 1)
        {
            if (player == 1)
            {
                action_screen(grid_P2, shots_P1, health_P2, &end);
            }
            else {
                waiting_screen(grid_P2, grid_P1, shots_P1, health_P2, &end);
            }
        }
        else {
            if (player == 2)
            {
                action_screen(grid_P1, shots_P2, health_P1, &end);
            }
            else {
                waiting_screen(grid_P1, grid_P2, shots_P2, health_P1, &end);
            }
        }
        turn = (turn == 1) ? 2 : 1;
    }
}
