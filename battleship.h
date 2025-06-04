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
void game_pause();

void initialize_grid(char grid[DIM][DIM]);
void display_grid(char grid[DIM][DIM], bool refresh);
void grid_to_string(char grid[DIM][DIM], char *buffer, size_t bufsize);
void string_to_grid(const char *buffer, char grid[DIM][DIM]);

int *get_coord(void);
char *coord_to_string(int *coord);

bool is_valid(int i, int j);
bool can_be_placed(int size, int rot, int i, int j, char grid[DIM][DIM]);
void place_ship(int size, int rot, int i, int j, char symbol, char grid[DIM][DIM]);
void placement(char grid[DIM][DIM], int player, Ship fleet[]);

bool shoot(char enemyGrid[DIM][DIM], char shotsGrid[DIM][DIM], int* shipHealth, int x, int y, bool mute);

void game_error(const char *message);

void action_screen(char grid[DIM][DIM], char shots[DIM][DIM], int *health, bool *end);
void waiting_screen(char grid[DIM][DIM], char grid_enemy[DIM][DIM], char shots[DIM][DIM], int *health, bool *end);

void play(int player, const char* ip_address, bool debug);

#endif // BATTLESHIP_H