/**
 * @file win/client_win.h
 * @brief Networking functions for Battleship client on Windows.
 *
 * Provides functions to connect to the server, send/receive messages, and manage connection state
 * using Winsock2. Used only on Windows platforms.
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

#define SERVER_PORT 5000 /**< TCP port for the Battleship server. */
#define MSG_LEN 256      /**< Maximum message length for network communication. */

/**
 * @brief Connects to the Battleship server.
 * @param server_ip The server's IP address.
 * @param debug Enable debug output if true.
 * @return Socket descriptor on success, or -1 on failure.
 */
int connect_to_server(const char *server_ip, bool debug);

/**
 * @brief Sends a message through the given socket.
 * @param socket The socket descriptor.
 * @param message The message to send.
 * @param debug Enable debug output if true.
 */
void send_message(int socket, const char *message, bool debug);

/**
 * @brief Receives a message from the server.
 * @param socket The socket descriptor.
 * @param buffer Buffer to store the received message.
 * @param bufsize Size of the buffer.
 * @param debug Enable debug output if true.
 * @return Number of bytes received, or -1 on error.
 */
int receive_message(int socket, char *buffer, int bufsize, bool debug);

/**
 * @brief Closes the connection to the server.
 * @param socket The socket descriptor.
 * @param debug Enable debug output if true.
 */
void close_connection(int socket, bool debug);

/**
 * @brief Sends information (e.g., player data) to a specified IP address.
 * @param ip_address The destination IP address.
 * @param message The message to send.
 * @param debug Enable debug output if true.
 */
void send_infos(const char *ip_address, const char *message, bool debug);

/**
 * @brief Tries repeatedly to send information to the server, with error handling.
 * @param ip_address The server IP address.
 * @param message The message to send.
 * @param debug Enable debug output if true.
 * @return True on success, false on failure.
 */
bool try_send_infos(const char *ip_address, const char *message, bool debug);

#endif // CLIENT_WIN_H

#endif