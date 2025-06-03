#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include "server.h"
#include "client.h"

#define DIM 11

// Ship struct definition
typedef struct {
    const char* name;
    char symbol;
    int size;
    int remaining;
    bool active;
} Ship;

// Externally visible functions
void initializeGrid(char grid[DIM][DIM][3]);
void displayGrid(char grid[DIM][DIM][3], bool hide);

int letterToIndex(char letter);

bool isValid(int i, int j);
bool canPlace(int size, int rot, int i, int j, char grid[DIM][DIM][3]);
void placeShip(int size, int rot, int i, int j, char symbol, char grid[DIM][DIM][3]);
void placement(char grid[DIM][DIM][3], int player, Ship fleet[]);

bool shoot(char enemyGrid[DIM][DIM][3], char shotsGrid[DIM][DIM][3], int* shipHealth);

void play(int player, const char* ip_address, bool debug);

#endif // BATTLESHIP_H