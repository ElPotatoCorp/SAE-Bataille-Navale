/**
 * @file linux/client_linux.h
 * @brief Networking functions for Battleship client on Linux.
 *
 * Provides functions to connect to the server, send/receive messages, and manage connection state
 * using POSIX sockets. Used only on Linux platforms.
 */

#ifdef __linux__

#ifndef CLIENT_LINUX_H
#define CLIENT_LINUX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT IPPORT_USERRESERVED /**< Default TCP port for Battleship server. */
#define MSG_LEN 256                    /**< Maximum message length for network communication. */

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

#endif // CLIENT_H

#endif