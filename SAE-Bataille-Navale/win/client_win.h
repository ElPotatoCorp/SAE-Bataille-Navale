/**
 * @file win/client_win.h
 * @brief Networking functions for Battleship client on Windows.
 *
 * This header provides declarations for connecting to a Battleship server,
 * sending and receiving messages, and managing the connection state using Winsock2.
 * Intended for use only on Windows platforms.
 *
 * Constants:
 *   - PORT:    TCP port for the Battleship server.
 *   - MSG_LEN: Maximum message length for network communication.
 *
 * Functions:
 *   - connect_to_server:   Initialize Winsock2 and connect to the server.
 *   - send_message:        Send a message to the server.
 *   - receive_message:     Receive a message from the server, with "404" disconnect detection.
 *   - close_connection:    Close the socket and clean up Winsock2.
 *
 * Usage:
 *   1. Use connect_to_server() to establish a connection.
 *   2. Use send_message() and receive_message() for communication.
 *   3. Use close_connection() when finished.
 *
 * @note This header and its implementation are intended for Windows only.
 */

#ifdef _WIN32

#ifndef CLIENT_WIN_H
#define CLIENT_WIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdbool.h>

#pragma comment(lib, "Ws2_32.lib")

#ifndef PORT
#define PORT 5000   /**< TCP port for the Battleship server. */
#endif

#ifndef MSG_LEN
#define MSG_LEN 256 /**< Maximum message length for network communication. */
#endif

 /**
  * @brief Attempts to connect to a Battleship server at the given IP address.
  *        Initializes Winsock2 automatically.
  * @param server_ip The server's IPv4 address as a string (e.g., "127.0.0.1").
  * @param debug Enable debug output if true.
  * @return The connected socket descriptor (>=0) on success, or -1 on failure.
  */
int connect_to_server(const char* server_ip, bool debug);

/**
 * @brief Sends a null-terminated message to the server over the given socket.
 * @param socket The connected socket descriptor.
 * @param message The message to send (null-terminated string).
 * @param debug Enable debug output if true.
 * @note Exits the process if sending fails or the socket is closed.
 */
void send_message(int socket, const char* message, bool debug);

/**
 * @brief Receives a message from the server into the provided buffer.
 * @param socket The connected socket descriptor.
 * @param buffer The buffer to store the received message (will be null-terminated).
 * @param bufsize The size of the buffer.
 * @param debug Enable debug output if true.
 * @return
 *    - Number of bytes received (>0) on success,
 *    - 0 if the socket was closed by the server,
 *    - 404 if the server sent "404" (disconnect message),
 *    - -1 on error.
 */
int receive_message(int socket, char* buffer, int bufsize, bool debug);

/**
 * @brief Closes the socket and cleans up Winsock2 state.
 * @param socket The socket descriptor to close.
 * @param debug Enable debug output if true.
 */
void close_connection(int socket, bool debug);

#endif // CLIENT_WIN_H

#endif