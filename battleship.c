#include "battleship.h"

void initializeGrid(char grid[DIM][DIM]) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            grid[i][j] = '-';
        }
    }
}

void displayGrid(char grid[DIM][DIM]) {
    printf("  1 2 3 4 5 6 7 8 9 0");
    for (int i = 0; i < DIM; i++) {
        printf("\n%c ", 'A' + i);
        for (int j = 0; j < DIM; j++) {
            printf("%c ", grid[i][j]);
        }
    }
    printf("\n");
}

// I did not verified if it works properly yet.
void gridToString(char grid[DIM][DIM], char *buffer, size_t bufsize) {
    int index = 0;
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (index < bufsize - 1) { // leave space for '\0'
                buffer[index++] = grid[i][j];
            }
        }
    }
    buffer[index] = '\0';
}

int letterToIndex(char letter) {
    letter = toupper(letter);
    return (letter >= 'A' && letter <= 'J') ? (letter - 'A') : 0;
}

bool isValid(int i, int j) {
    return i >= 0 && i < DIM && j >= 0 && j < DIM;
}

bool canPlace(int size, int rot, int i, int j, char grid[DIM][DIM]) {
    for (int k = 0; k < size; ++k) {
        int x = i + (rot == 3 ? k : rot == 1 ? -k : 0);
        int y = j + (rot == 2 ? k : rot == 4 ? -k : 0);
        if (!isValid(x, y) || grid[x][y] != '-') return false;
    }
    return true;
}

void placeShip(int size, int rot, int i, int j, char symbol, char grid[DIM][DIM]) {
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

        displayGrid(grid);
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
        int i = letterToIndex(letter);

        printf("Number (0-9): ");
        int j;
        scanf("%d", &j);
        j -= 1;
        if (j == -1)
            j = 9;

        printf("Rotation (1=north, 2=east, 3=south, 4=west): ");
        int rot;
        scanf("%d", &rot);

        if (!canPlace(fleet[choice].size, rot, i, j, grid)) {
            printf("Invalid placement.\n");
            continue;
        }

        placeShip(fleet[choice].size, rot, i, j, fleet[choice].symbol, grid);
        fleet[choice].active = false;
        system("cls || clear");
    }
}

bool shoot(char enemyGrid[DIM][DIM], char shotsGrid[DIM][DIM], int* shipHealth) {
    printf("Letter (A-J): ");
    char letter;
    scanf(" %c", &letter, 1);
    int i = letterToIndex(letter);

    printf("Number (0-9): ");
    int j;
    scanf("%d", &j);
    j -= 1;
    if (j == -1)
        j = 9;

    if (!isValid(i, j)) {
        printf("Invalid coordinates.\n");
        return false;
    }

    if (shotsGrid[i][j] != '-') {
        printf("You already fired here.\n");
        return false;
    }

    if (enemyGrid[i][j] != '-') {
        printf("Hit!\n");
        shotsGrid[i][j] = 'X';
        char symbol = enemyGrid[i][j];
        enemyGrid[i][j] = 'X';
        shipHealth[symbol]--;
        if (shipHealth[symbol] == 0)
        {
            printf("Ship sunk!\n");
        }
        return true;
    }
    else {
        printf("Miss.\n");
        shotsGrid[i][j] = 'O';
        return false;
    }
}


void play(int player, const char* ip_address, bool debug) {
    char gridP1[DIM][DIM], gridP2[DIM][DIM];
    char shotsP1[DIM][DIM], shotsP2[DIM][DIM];
    initializeGrid(gridP1);
    initializeGrid(gridP2);
    initializeGrid(shotsP1);
    initializeGrid(shotsP2);

    Ship fleetP1[5] = { {"Carrier",'#',5,5,true},{"Battleship",'@',4,4,true},{"Cruiser",'%',3,3,true},{"Submarine",'&',3,3,true},{"Destroyer",'$',2,2,true} };
    Ship fleetP2[5];
    memcpy(fleetP2, fleetP1, sizeof(fleetP1));

    int healthP1[128] = { 0 }, healthP2[128] = { 0 };
    healthP1['#'] = 5; healthP1['@'] = 4; healthP1['%'] = 3; healthP1['&'] = 3; healthP1['$'] = 2;
    memcpy(healthP2, healthP1, sizeof(healthP1));

    placement(gridP1, 1, fleetP1);
    placement(gridP2, 2, fleetP2);

    int turn = 1;
    while (true) {
        system("cls || clear");
        printf("Player %d's turn\n", turn);
        if (turn == 1) {
            printf("Your shots grid:\n");
            displayGrid(shotsP1);
            shoot(gridP2, shotsP1, healthP2);
            if (healthP2['#'] == 0 && healthP2['@'] == 0 && healthP2['%'] == 0 && healthP2['&'] == 0 && healthP2['$'] == 0) {
                printf("Player 1 won!\n");
                break;
            }
        }
        else {
            printf("Your shots grid:\n");
            displayGrid(shotsP2);
            shoot(gridP1, shotsP2, healthP1);
            if (healthP1['#'] == 0 && healthP1['@'] == 0 && healthP1['%'] == 0 && healthP1['&'] == 0 && healthP1['$'] == 0) {
                printf("Player 2 won!\n");
                break;
            }
        }
        turn = 3 - turn;
        printf("Press Enter to continue...\n");
        getchar(); getchar();
    }
}

/*
void play(int player, const char* ip_address, bool debug) {
    char gridP1[DIM][DIM], gridP2[DIM][DIM];
    char shotsP1[DIM][DIM], shotsP2[DIM][DIM];
    if (player == 1) {
        initializeGrid(gridP1);
        initializeGrid(shotsP1);
    }
    else {
        initializeGrid(gridP2);
        initializeGrid(shotsP2);
    }

    Ship fleetP1[5] = { {"Carrier",'#',5,5,true},{"Battleship",'@',4,4,true},{"Cruiser",'%',3,3,true},{"Submarine",'&',3,3,true},{"Destroyer",'$',2,2,true} };
    Ship fleetP2[5];
    memcpy(fleetP2, fleetP1, sizeof(fleetP1));

    int healthP1[128] = { 0 }, healthP2[128] = { 0 };
    healthP1['#'] = 5; healthP1['@'] = 4; healthP1['%'] = 3; healthP1['&'] = 3; healthP1['$'] = 2;
    memcpy(healthP2, healthP1, sizeof(healthP1));

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
        turn = atoi(recv_infos());
        printf("Turn: %d\n", turn);
    }

    if (player == 1) {
        placement(gridP1, 1, fleetP1);
        try_send_infos(gridP1);
        recv_infos();
    }

}
*/