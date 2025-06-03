#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h> // Use standard bool type
#include "server.h"
#include "client.h"

#define DIM 11
const char* ALPHA[] = { "A","B","C","D","E","F","G","H","I","J" };

typedef struct {
    const char* name;
    char symbol;
    int size;
    int remaining;
    bool active;
} Ship;

void initializeGrid(char grid[DIM][DIM][3]) {
    strcpy(grid[0][0], " ");
    for (int i = 1; i < DIM; i++) {
        snprintf(grid[0][i], 3, "%d", i);
        strcpy(grid[i][0], ALPHA[i - 1]);
    }
    for (int i = 1; i < DIM; i++) {
        for (int j = 1; j < DIM; j++) {
            strcpy(grid[i][j], "-");
        }
    }
}

void displayGrid(char grid[DIM][DIM][3], bool hide) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (hide && (strcmp(grid[i][j], "#") == 0 || strcmp(grid[i][j], "@") == 0 ||
                strcmp(grid[i][j], "%") == 0 || strcmp(grid[i][j], "&") == 0 ||
                strcmp(grid[i][j], "$") == 0))
                printf("- ");
            else
                printf("%s ", grid[i][j]);
        }
        printf("\n");
    }
}

int letterToIndex(char letter) {
    letter = toupper(letter);
    return (letter >= 'A' && letter <= 'J') ? (letter - 'A' + 1) : 0;
}

bool isValid(int i, int j) {
    return i >= 1 && i < DIM && j >= 1 && j < DIM;
}

bool canPlace(int size, int rot, int i, int j, char grid[DIM][DIM][3]) {
    for (int k = 0; k < size; ++k) {
        int x = i + (rot == 3 ? k : rot == 1 ? -k : 0);
        int y = j + (rot == 2 ? k : rot == 4 ? -k : 0);
        if (!isValid(x, y) || strcmp(grid[x][y], "-") != 0) return false;
    }
    return true;
}

void placeShip(int size, int rot, int i, int j, char symbol, char grid[DIM][DIM][3]) {
    for (int k = 0; k < size; ++k) {
        int x = i + (rot == 3 ? k : rot == 1 ? -k : 0);
        int y = j + (rot == 2 ? k : rot == 4 ? -k : 0);
        grid[x][y][0] = symbol;
        grid[x][y][1] = '\0';
    }
}

void placement(char grid[DIM][DIM][3], int player, Ship fleet[]) {
    printf("Placement of Player %d's ships\n", player);
    while (true) {
        bool finished = true;
        for (int k = 0; k < 5; k++) {
            if (fleet[k].active) finished = false;
        }
        if (finished) break;

        displayGrid(grid, false);
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

        printf("Number (1-10): ");
        int j;
        scanf("%d", &j);

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

bool shoot(char enemyGrid[DIM][DIM][3], char shotsGrid[DIM][DIM][3], int* shipHealth) {
    printf("Letter (A-J): ");
    char letter;
    scanf(" %c", &letter, 1);
    int i = letterToIndex(letter);

    printf("Number (1-10): ");
    int j;
    scanf("%d", &j);

    if (!isValid(i, j)) {
        printf("Invalid coordinates.\n");
        return false;
    }

    if (strcmp(shotsGrid[i][j], "-") != 0) {
        printf("You already fired here.\n");
        return false;
    }

    if (strcmp(enemyGrid[i][j], "-") != 0 && strcmp(enemyGrid[i][j], "X") != 0) {
        printf("Hit!\n");
        shotsGrid[i][j][0] = 'X'; shotsGrid[i][j][1] = '\0';
        char symbol = enemyGrid[i][j][0];
        enemyGrid[i][j][0] = 'X'; enemyGrid[i][j][1] = '\0';
        shipHealth[symbol]++;
        if (shipHealth[symbol] == 0) printf("Ship sunk!\n");
        return true;
    }
    else {
        printf("Miss.\n");
        shotsGrid[i][j][0] = 'O'; shotsGrid[i][j][1] = '\0';
        return false;
    }
}

/*
void play(int player, const char* ip_address) {
    char gridP1[DIM][DIM][3], gridP2[DIM][DIM][3];
    char shotsP1[DIM][DIM][3], shotsP2[DIM][DIM][3];
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
            displayGrid(shotsP1, false);
            shoot(gridP2, shotsP1, healthP2);
            if (healthP2['#'] == 0 && healthP2['@'] == 0 && healthP2['%'] == 0 && healthP2['&'] == 0 && healthP2['$'] == 0) {
                printf("Player 1 won!\n");
                break;
            }
        }
        else {
            printf("Your shots grid:\n");
            displayGrid(shotsP2, false);
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
*/

void play(int player, const char* ip_address) {
    char gridP1[DIM][DIM][3], gridP2[DIM][DIM][3];
    char shotsP1[DIM][DIM][3], shotsP2[DIM][DIM][3];
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

    int turn;
    if (player == 1) {
        turn = rand() % 2 + 1;    
        send_infos(ip_address, to_string(turn));
        printf("Turn: %d\n", turn);
    }
    else {
        turn = atoi(recv_infos());
        printf("Turn: %d\n", turn);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    const char *ip_address = argv[1];
    int player = atoi(argv[2]);

    play(player, ip_address);
    return 0;
}