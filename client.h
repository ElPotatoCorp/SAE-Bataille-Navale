#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT IPPORT_USERRESERVED
#define MSG_LEN 256

int connect_to_server(const char *server_ip);
void send_message(int socket, const char *message);
void receive_message(int socket, char *buffer, int bufsize);
void close_connection(int socket);

void send_infos(const char *ip_address, const char *message);
void try_send_infos(const char *ip_address, const char *message);

#endif // CLIENT_H