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

#define DIM 10

// Ship struct definition
typedef struct {
    const char* name;
    char symbol;
    int size;
    int remaining;
    bool active;
} Ship;

// Externally visible functions
void initializeGrid(char grid[DIM][DIM]);
void displayGrid(char grid[DIM][DIM]);
void gridToString(char grid[DIM][DIM], char *buffer, size_t bufsize);

int letterToIndex(char letter);

bool isValid(int i, int j);
bool canPlace(int size, int rot, int i, int j, char grid[DIM][DIM]);
void placeShip(int size, int rot, int i, int j, char symbol, char grid[DIM][DIM]);
void placement(char grid[DIM][DIM], int player, Ship fleet[]);

bool shoot(char enemyGrid[DIM][DIM], char shotsGrid[DIM][DIM], int* shipHealth);

void play(int player, const char* ip_address, bool debug);

#endif // BATTLESHIP_H