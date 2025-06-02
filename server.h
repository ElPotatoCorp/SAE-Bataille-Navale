#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT IPPORT_USERRESERVED
#define MSG_LEN 256

extern int listenSocket;
extern int server_should_run;

void handle_sigint(int sig);
void stop_server();
int create_listening_socket();
void handle_client(int clientSocket);
void start_server();

#endif // SERVER_H