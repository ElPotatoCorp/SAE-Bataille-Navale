#include "battleship.h"

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

int letter_to_index(char letter) {
    letter = toupper(letter);
    return (letter >= 'A' && letter <= 'J') ? (letter - 'A') : 0;
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
        bool finished = true;
        for (int k = 0; k < 5; k++) {
            if (fleet[k].active) finished = false;
        }
        if (finished) break;

        display_grid(grid);
        printf("Choose a ship (1-5):\n");
        for (int i = 0; i < 5; i++) {
            if (fleet[i].active)
                printf("%d. %s - size: %d - symbol: '%c'\n", i + 1, fleet[i].name, fleet[i].size, fleet[i].symbol);
        }

        int choice;
        scanf("%d", &choice);
        choice--;
        if (choice < 0 || choice >= 5 || !fleet[choice].active) continue;

        printf("Letter (A-J): ");
        char letter;
        scanf(" %c", &letter, 1);
        int i = letter_to_index(letter);

        printf("Number (0-9): ");
        int j;
        scanf("%d", &j);
        j -= 1;
        if (j == -1)
            j = 9;

        printf("Rotation (1=north, 2=east, 3=south, 4=west): ");
        int rot;
        scanf("%d", &rot);

        if (!can_be_placed(fleet[choice].size, rot, i, j, grid)) {
            printf("Invalid placement.\n");
            continue;
        }

        place_ship(fleet[choice].size, rot, i, j, fleet[choice].symbol, grid);
        fleet[choice].active = false;
        system("cls || clear");
    }
}

bool shoot(char enemy_grid[DIM][DIM], char shots_grid[DIM][DIM], int* ship_health) {
    printf("Letter (A-J): ");
    char letter;
    scanf(" %c", &letter, 1);
    int i = letter_to_index(letter);

    printf("Number (0-9): ");
    int j;
    scanf("%d", &j);
    j -= 1;
    if (j == -1)
        j = 9;

    if (!is_valid(i, j)) {
        printf("Invalid coordinates.\n");
        return false;
    }

    if (shots_grid[i][j] != '-') {
        printf("You already fired here.\n");
        return false;
    }

    if (enemy_grid[i][j] != '-') {
        printf("Hit!\n");
        shots_grid[i][j] = 'X';
        char symbol = enemy_grid[i][j];
        enemy_grid[i][j] = 'X';
        ship_health[symbol]--;
        if (ship_health[symbol] == 0)
        {
            printf("Ship sunk!\n");
        }
        return true;
    }
    else {
        printf("Miss.\n");
        shots_grid[i][j] = 'O';
        return false;
    }
}

void play(int player, const char* ip_address, bool debug) {
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

        char turn_str;
        sprintf(&turn_str, "%d", turn);

        printf("Waiting for other player to connect...\n");
        try_send_infos(ip_address, &turn_str, debug);
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

        printf("Waiting for other player...\n");
        try_send_infos(ip_address, grid_str, debug);
        string_to_grid(recv_infos(), grid_P2);

        display_grid(grid_P2);
    }
    else {
        placement(grid_P2, 2, fleet_P2);
        grid_to_string(grid_P2, grid_str, sizeof(grid_str));

        printf("Waiting for other player...\n");
        string_to_grid(recv_infos(), grid_P1);
        send_infos(ip_address, grid_str, debug);

        display_grid(grid_P1);
    }

}
