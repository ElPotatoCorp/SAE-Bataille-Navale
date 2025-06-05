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

#define SERVER_PORT IPPORT_USERRESERVED
#define MSG_LEN 256

int connect_to_server(const char *server_ip, bool debug);
void send_message(int socket, const char *message, bool debug);
int receive_message(int socket, char *buffer, int bufsize, bool debug);
void close_connection(int socket, bool debug);

void send_infos(const char *ip_address, const char *message, bool debug);
bool try_send_infos(const char *ip_address, const char *message, bool debug);

#endif // CLIENT_H

#endif