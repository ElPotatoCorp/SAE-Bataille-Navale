/**
 * @file win/server_win.h
 * @brief Networking functions for the Battleship relay server on Windows.
 *
 * This header provides declarations for initializing, running, and cleaning up a Battleship relay server using Winsock2. The relay server:
 *   - Accepts two player connections.
 *   - Exchanges initial player numbers.
 *   - Relays all messages between players.
 *   - Handles client disconnects and server interruption (e.g., Ctrl+C) by notifying the remaining player with "404".
 *
 * @note This header and its implementation are intended for Windows only.
 */

#ifdef _WIN32

#ifndef SERVER_WINDOWS_H
#define SERVER_WINDOWS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32.lib")

#ifndef PORT
#define PORT 5000   /**< TCP port for the Battleship server. */
#endif

#ifndef MSG_LEN
#define MSG_LEN 256 /**< Maximum message length for network communication. */
#endif

 /**
  * @brief Prints an error message, cleans up Winsock, and exits the program.
  * @param msg The error message to print.
  */
void error_exit(const char* msg);

/**
 * @brief Initializes Winsock2 for network operations.
 */
void initialize_winsock(void);

/**
 * @brief Creates, binds, and listens on a TCP socket for Battleship server.
 * @return The listening socket descriptor.
 */
SOCKET create_listening_socket(void);

/**
 * @brief Accepts a single client connection on the given listening socket.
 * @param server_fd The listening socket descriptor.
 * @param player_name A string to identify the player in logs/messages.
 * @param debug Enable debug output if true.
 * @return The accepted client socket descriptor.
 */
SOCKET accept_client(SOCKET server_fd, const char* player_name, bool debug);

/**
 * @brief Sends a message to a client socket.
 * @param client_fd The client socket descriptor.
 * @param message The message to send (null-terminated string).
 * @param debug Enable debug output if true.
 */
void send_server_message(SOCKET client_fd, char* message, bool debug);

/**
 * @brief Receives a message from a client socket.
 * @param client_fd The client socket descriptor.
 * @param buffer Buffer to store the received message (must be at least MSG_LEN bytes).
 * @param debug Enable debug output if true.
 * @return Number of bytes received, or -1 on error/disconnect.
 */
int receive_client_message(SOCKET client_fd, char* buffer, int bufsize, int debug);

/**
 * @brief Sends a "404" disconnect message to a socket and closes it.
 * @param fd The socket descriptor to notify and close.
 */
void send_404_and_close(SOCKET fd);

/**
 * @brief Signal handler for SIGINT (Ctrl+C); notifies both clients with "404" and exits.
 * @param signum The signal number (unused).
 */
void on_sigint(int signum);

/**
 * @brief Relays a message from source_fd to dest_fd.
 * @param source_fd The socket to receive from.
 * @param dest_fd The socket to send to.
 * @return Number of bytes relayed, or -1 on error/disconnect.
 */
int relay_message(SOCKET source_fd, SOCKET dest_fd);

/**
 * @brief Main relay loop: forwards messages between two clients, and handles disconnects.
 * @param player1_fd The first player's socket descriptor.
 * @param player2_fd The second player's socket descriptor.
 */
void relay_loop(SOCKET player1_fd, SOCKET player2_fd);

/**
 * @brief Closes the provided sockets and cleans up Winsock state.
 * @param fd1 First socket descriptor.
 * @param fd2 Second socket descriptor.
 * @param server_fd Listening/server socket descriptor.
 */
void cleanup(SOCKET fd1, SOCKET fd2, SOCKET server_fd);

#endif // SERVER_WINDOWS_H

#endif // _WIN32