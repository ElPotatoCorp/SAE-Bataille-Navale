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
#define INVALID_SOCKET -1
#endif // INVALID_SOCKET

#ifndef PORT
#define PORT IPPORT_USERRESERVED	/**< TCP port for the Battleship server. */
#endif // PORT

#ifndef MSG_LEN
#define MSG_LEN 256					/**< Maximum message length for network communication. */
#endif // MSG_LEN

void error_exit(const char* msg);
int create_listening_socket(void);
int accept_client(int server_fd, const char* player_name, bool debug);
void send_server_message(int client_fd, char* message, bool debug);
void send_404_and_close(int fd);
void on_sigint(int signum);
int relay_message(int source_fd, int dest_fd);
void relay_loop(int player1_fd, int player2_fd);
void cleanup(int fd1, int fd2, int server_fd);

#endif // SERVER_LINUX_H

#endif // __linux__