/**
 * @file linux/server_linux.h
 * @brief Networking functions for Battleship server on Linux.
 *
 * Provides setup, listening, and data reception for the Battleship server using POSIX sockets.
 * Used only on Linux platforms.
 */

#ifdef __linux__

#ifndef SERVER_LINUX_H
#define SERVER_LINUX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT IPPORT_USERRESERVED /**< Default TCP port for Battleship server. */
#define MSG_LEN 256              /**< Maximum message length for network communication. */

extern int listenSocket; /**< Listening socket for incoming client connections. */

/**
 * @brief Creates and configures the listening socket for the server.
 * @return The socket descriptor on success, or -1 on failure.
 */
int create_listening_socket(void);

/**
 * @brief Receives information from a connected client.
 * @return Pointer to a static buffer containing the received message.
 */
const char *recv_infos(void);

#endif // SERVER_H

#endif