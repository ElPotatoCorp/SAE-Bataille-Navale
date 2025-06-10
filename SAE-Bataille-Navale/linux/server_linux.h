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

#define PORT IPPORT_USERRESERVED
#define MSG_LEN 256

extern int listenSocket;
int create_listening_socket(void);
const char *recv_infos(void);

#endif // SERVER_H

#endif