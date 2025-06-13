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
 * @brief Displays a single grid cell with optional highlighting.
 * @param c The character to display in the cell.
 * @param highlight If true, highlights the cell (e.g., with color).
 */
void display_box(char c, bool highlight);

/**
 * @brief Displays the grid to the console.
 * @param grid The grid to display.
 * @param x The X coordinate.
 * @param y The Y coordinate.
 */
void display_grid(char grid[DIM][DIM], int x, int y);

/**
 * @brief Displays the board to the console.
 * @param fleet_grid The player's fleet grid.
 * @param shot_grid The player's shot grid.
 * @param x The X coordinate for the player's fleet grid.
 * @param y The Y coordinate for the player's fleet grid.
 * @param x_bis The X coordinate for the player's shot grid.
 * @param y_bis The Y coordinate for the player's shot grid.
 */
void display_board(char fleet_grid[DIM][DIM], char shot_grid[DIM][DIM], int x, int y, int x_bis, int y_bis);

/**
 * @brief Displays the final board with the aid of player's shots and enemy's shots.
 * @param shot The player's shot grid.
 * @param shot_enemy The enemy's shot grid.
 */
void display_final_board(char shot[DIM][DIM], char shot_enemy[DIM][DIM]);

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
 * @param coord A 2-element int array [i, j].
 */
void get_coord(int coord[2]);

/**
 * @brief Checks if a ship can be placed at a given location and orientation.
 * @param size Ship length.
 * @param rot 0 = horizontal, 1 = vertical.
 * @param i Row index.
 * @param j Column index.
 * @param grid The player's fleet grid.
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
 * @param grid The player's fleet grid.
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
 */
void shoot(char enemyGrid[DIM][DIM], char shotsGrid[DIM][DIM], int* shipHealth, int x, int y, bool mute);

/**
 * @brief Prints a game error message and handles error state.
 * @param socket The socket descriptor.
 * @param buffer Buffer to store the received message.
 * @param bufsize Size of the buffer.
 * @param message The error message.
 */
void server_communication_handler(int socket, char* buffer, int bufsize, const char* message);

/**
 * @brief Sends a message to the opponent.
 * @param info The message to send.
 */
void send_info_to_opponent(char* info);

/**
 * @brief Receives a message from the opponent.
 * @param buffer Buffer to store the received message.
 * @param bufsize Size of the buffer.
 * @param error_message Error message to display if reception fails.
 */
void receive_info_from_opponent(char* buffer, int bufsize, const char* error_message);

/**
 * @brief Displays the player's placement screen.
 * @param grid The player's fleet grid.
 * @param fleet Array of ships.
 * @param enemy_grid The opponent's grid.
 */
void placement_screen(char grid[DIM][DIM], Ship fleet[5], char enemy_grid[DIM][DIM]);

/**
 * @brief Displays the game over screen with the final results.
 * @param shots The player's shot grid.
 * @param shots_enemy The enemy's shot grid.
 * @param message Message to display (e.g., "You win!" or "You lose!").
 * @param player Player number (1 or 2).
 */
void game_over_screen(char shots[DIM][DIM], char shots_enemy[DIM][DIM], char* message, int player);

/**
 * @brief Displays the player's action screen and processes input.
 * @param grid The player's fleet grid.
 * @param shots The player's shot grid.
 * @param grid_enemy The enemy's fleet grid.
 * @param shots_enemy The enemy's shot grid.
 * @param health Ship health array.
 */
void action_screen(char grid[DIM][DIM], char shots[DIM][DIM], char grid_enemy[DIM][DIM], char shots_enemy[DIM][DIM], int *health);

/**
 * @brief Displays a waiting screen between player turns.
 * @param grid The player's fleet grid.
 * @param shots The player's shot grid.
 * @param grid_enemy The enemy's fleet grid.
 * @param shots_enemy The enemy's shot grid.
 * @param health Ship health array.
 */
void waiting_screen(char grid[DIM][DIM], char shots[DIM][DIM], char grid_enemy[DIM][DIM], char shots_enemy[DIM][DIM], int *health);

/**
 * @brief Main game loop controller.
 * @param player Player number.
 * @param restarted If true, the game is restarted.
 * @param host_mode Host mode flag.
 * @param ip_address IP address of the server (for network play).
 * @param debug Enable debug output if true.
 */
void play(const char *ip_address, bool restarted, bool host_mode, bool debug);

#endif // BATTLESHIP_H