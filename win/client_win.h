#ifndef CLIENT_WIN_H
#define CLIENT_WIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdbool.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 5000
#define MSG_LEN 256

int connect_to_server(const char *server_ip, bool debug);
void send_message(int socket, const char *message, bool debug);
int receive_message(int socket, char *buffer, int bufsize, bool debug);
void close_connection(int socket, bool debug);

void send_infos(const char *ip_address, const char *message, bool debug);
bool try_send_infos(const char *ip_address, const char *message, bool debug);

#endif // CLIENT_WIN_H