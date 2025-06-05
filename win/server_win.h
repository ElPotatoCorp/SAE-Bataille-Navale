#ifndef SERVER_WIN_H
#define SERVER_WIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdbool.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000
#define MSG_LEN 256

extern int listenSocket;
int create_listening_socket(void);
const char *recv_infos(void);

#endif // SERVER_WIN_H