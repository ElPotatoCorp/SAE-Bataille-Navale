/**
 * @file battleship.h
 * @brief Core game logic, data structures, and utility functions for Battleship.
 *
 * This header declares the main types, constants, and functions used for managing
 * the Battleship grid, fleet, game actions, and player interactions.
 */

#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#ifdef _WIN32
#include "win/client_win.h"
#include "win/server_win.h"
#elif __linux__
#include "linux/client_linux.h"
#include "linux/server_linux.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define DIM 10 /**< The size of the grid (10x10). */

 /**
  * @struct Ship
  * @brief Represents a ship in the fleet.
  * @var Ship::name Ship's name (e.g., "Battleship")
  * @var Ship::symbol Character symbol representing the ship on the grid.
  * @var Ship::size Length of the ship (number of grid cells).
  * @var Ship::remaining Number of remaining (unhit) segments.
  * @var Ship::active Ship is still afloat (true) or sunk (false).
  */
typedef struct {
    const char* name;
    char symbol;
    int size;
    int remaining;
    bool active;
} Ship;

/**
 * @brief Clears the screen (platform-specific).
 */
void clear(void);

/**
 * @brief Pauses the game until user input.
 */
void game_pause(void);

/**
 * @brief Initializes the grid to empty cells.
 * @param grid The grid to be initialized.
 */
void initialize_grid(char grid[DIM][DIM]);

/**
 * @brief Displays the grid to the console.
 * @param grid The grid to display.
 * @param refresh If true, clears the screen before displaying.
 */
void display_grid(char grid[DIM][DIM]);

/**
 * @brief Converts the grid to a string buffer.
 * @param grid The grid to convert.
 * @param buffer The output buffer.
 * @param bufsize The buffer size.
 */
void grid_to_string(char grid[DIM][DIM], char* buffer, size_t bufsize);

/**
 * @brief Loads a grid from a string buffer.
 * @param buffer String containing the grid state.
 * @param grid The output grid.
 */
void string_to_grid(const char* buffer, char grid[DIM][DIM]);

/**
 * @brief Prompts for and returns a pointer to a coordinate (i, j).
 * @return Pointer to a 2-element int array [i, j].
 */
int *get_coord(void);

/**
 * @brief Converts a coordinate to a string (e.g., "A5").
 * @param coord Pointer to a 2-element int array [i, j].
 * @return String representation of the coordinate.
 */
char *coord_to_string(int* coord);

/**
 * @brief Checks if a coordinate is valid on the grid.
 * @param i Row index.
 * @param j Column index.
 * @return True if coordinate is within bounds.
 */
bool is_valid(int i, int j);

/**
 * @brief Checks if a ship can be placed at a given location and orientation.
 * @param size Ship length.
 * @param rot 0 = horizontal, 1 = vertical.
 * @param i Row index.
 * @param j Column index.
 * @param grid The grid.
 * @return True if placement is possible.
 */
bool can_be_placed(int size, int rot, int i, int j, char grid[DIM][DIM]);

/**
 * @brief Places a ship on the grid.
 * @param size Ship length.
 * @param rot 0 = horizontal, 1 = vertical.
 * @param i Row index.
 * @param j Column index.
 * @param symbol Ship symbol.
 * @param grid The grid.
 */
void place_ship(int size, int rot, int i, int j, char symbol, char grid[DIM][DIM]);

/**
 * @brief Handles ship placement for a player.
 * @param grid The player's grid.
 * @param player Player number.
 * @param fleet Array of ships.
 */
void placement(char grid[DIM][DIM], int player, Ship fleet[]);

/**
 * @brief Processes a shot at the enemy grid.
 * @param enemyGrid The opponent's grid.
 * @param shotsGrid The player's shot tracking grid.
 * @param shipHealth Array of remaining health per ship.
 * @param x Row index to shoot.
 * @param y Column index to shoot.
 * @param mute If true, disables output.
 * @return True if a ship was hit.
 */
bool shoot(char enemyGrid[DIM][DIM], char shotsGrid[DIM][DIM], int* shipHealth, int x, int y, bool mute);

/**
 * @brief Prints a game error message and handles error state.
 * @param socket The socket descriptor.
 * @param buffer Buffer to store the received message.
 * @param bufsize Size of the buffer.
 * @param message The error message.
 */
void server_communication_handler(int socket, char* buffer, int bufsize, const char* message);

/**
 * @brief Displays the player's placement screen.
 * @param grid The player's grid.
 * @param fleet Array of ships.
 * @param enemy_grid The opponent's grid.
 */
void placement_screen(char grid[DIM][DIM], Ship fleet[5], char enemy_grid[DIM][DIM]);

/**
 * @brief Displays the player's action screen and processes input.
 * @param grid The player's grid.
 * @param shots The player's shot grid.
 * @param health Ship health array.
 * @param end Pointer to game end flag.
 */
void action_screen(char grid[DIM][DIM], char shots[DIM][DIM], int* health, bool* end);

/**
 * @brief Displays a waiting screen between player turns.
 * @param grid The player's grid.
 * @param grid_enemy The enemy's grid.
 * @param shots The player's shot grid.
 * @param health Ship health array.
 * @param end Pointer to game end flag.
 */
void waiting_screen(char grid[DIM][DIM], char grid_enemy[DIM][DIM], char shots[DIM][DIM], int *health, bool *end);

/**
 * @brief Main game loop controller.
 * @param player Player number.
 * @param ip_address IP address of the server (for network play).
 * @param debug Enable debug output if true.
 */
void play(const char *ip_address, bool debug);

#endif // BATTLESHIP_H