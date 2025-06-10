/**
 * @file win/server_win.h
 * @brief Networking functions for Battleship server on Windows.
 *
 * Provides setup, listening, and data reception for the Battleship server using Winsock2.
 * Used only on Windows platforms.
 */

#ifdef _WIN32

#ifndef SERVER_WIN_H
#define SERVER_WIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdbool.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000      /**< TCP port for the Battleship server. */
#define MSG_LEN 256    /**< Maximum message length for network communication. */

extern SOCKET listenSocket; /**< Listening socket for incoming client connections. */

/**
 * @brief Creates and configures the listening socket for the server.
 * @return The socket descriptor on success, or -1 on failure.
 */
int create_listening_socket(void);

/**
 * @brief Receives information from a connected client.
 * @return Pointer to a static buffer containing the received message.
 */
const char* recv_infos(void);

#endif // SERVER_WIN_H

#endif