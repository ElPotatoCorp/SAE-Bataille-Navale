/**
 * @file server_linux.h
 * @brief Networking functions for the Battleship relay server on Linux.
 *
 * This header provides declarations for initializing, running, and cleaning up a Battleship relay server using POSIX sockets. The relay server:
 *   - Accepts two player connections.
 *   - Exchanges initial player numbers.
 *   - Relays all messages between players.
 *   - Handles client disconnects and server interruption (e.g., Ctrl+C) by notifying the remaining player with "404".
 *
 * @note This header and its implementation are intended for Linux only.
 */

#ifdef __linux__

#ifndef SERVER_LINUX_H
#define SERVER_LINUX_H

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1 /**< POSIX invalid socket value. */
#endif // INVALID_SOCKET

#ifndef PORT
#define PORT IPPORT_USERRESERVED	/**< TCP port for the Battleship server. */
#endif // PORT

#ifndef MSG_LEN
#define MSG_LEN 256					/**< Maximum message length for network communication. */
#endif // MSG_LEN

 /**
  * @brief Prints an error message and exits the program.
  * @param msg The error message to print.
  */
void error_exit(const char* msg);

/**
 * @brief Creates, binds, and listens on a TCP socket for Battleship server.
 * @return The listening socket descriptor.
 */
int create_listening_socket(void);

/**
 * @brief Accepts a single client connection on the given listening socket.
 * @param server_fd The listening socket descriptor.
 * @param player_name A string to identify the player in logs/messages.
 * @param debug Enable debug output if true.
 * @return The accepted client socket descriptor.
 */
int accept_client(int server_fd, const char* player_name, bool debug);

/**
 * @brief Sends a message to a client socket.
 * @param client_fd The client socket descriptor.
 * @param message The message to send (null-terminated string).
 * @param debug Enable debug output if true.
 */
void send_server_message(int client_fd, char* message, bool debug);

/**
 * @brief Receives a message from a client socket.
 * @param client_fd The client socket descriptor.
 * @param buffer Buffer to store the received message (must be at least bufsize bytes).
 * @param bufsize The size of the provided buffer.
 * @param debug Enable debug output if true.
 * @return Number of bytes received, 0 if connection closed, or -1 on error.
 */
int receive_client_message(int client_fd, char* buffer, int bufsize, int debug);

/**
 * @brief Sends a "404" disconnect message to a socket and closes it.
 * @param fd The socket descriptor to notify and close.
 */
void send_404_and_close(int fd);

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
int relay_message(int source_fd, int dest_fd);

/**
 * @brief Main relay loop: forwards messages between two clients, and handles disconnects.
 * @param player1_fd The first player's socket descriptor.
 * @param player2_fd The second player's socket descriptor.
 */
void relay_loop(int player1_fd, int player2_fd);

/**
 * @brief Closes the provided sockets.
 * @param fd1 First socket descriptor.
 * @param fd2 Second socket descriptor.
 * @param server_fd Listening/server socket descriptor.
 */
void cleanup(int fd1, int fd2, int server_fd);

#endif // SERVER_LINUX_H

#endif // __linux__