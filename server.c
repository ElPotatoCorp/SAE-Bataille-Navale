#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

int listenSocket = -1;
int server_should_run = 1;

void handle_sigint(int sig) {
    printf("\nServer shutting down (SIGINT received)...\n");
    server_should_run = 0;
    if (listenSocket != -1) close(listenSocket);
    exit(0);
}

void stop_server() {
    server_should_run = 0;
    if (listenSocket != -1) close(listenSocket);
    printf("Server stopped gracefully.\n");
}

int create_listening_socket() {
    int sock;
    struct sockaddr_in localAddress;
    socklen_t addressLength = sizeof(struct sockaddr_in);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(-1);
    }
    printf("Socket created successfully! (%d)\n", sock);

    memset(&localAddress, 0x00, addressLength);
    localAddress.sin_family = PF_INET;
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddress.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&localAddress, addressLength) < 0) {
        perror("bind");
        exit(-2);
    }
    printf("Socket successfully bound!\n");

    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(-3);
    }
    printf("Socket set to passive listening mode ...\n");

    return sock;
}

void handle_client(int clientSocket) {
    char sendBuffer[MSG_LEN];
    char recvBuffer[MSG_LEN];
    int bytesWritten, bytesRead;

    memset(sendBuffer, 0x00, MSG_LEN * sizeof(char));
    memset(recvBuffer, 0x00, MSG_LEN * sizeof(char));

    // Receive data from the client
    bytesRead = read(clientSocket, recvBuffer, MSG_LEN * sizeof(char));
    switch (bytesRead) {
        case -1:
            perror("read");
            close(clientSocket);
            return;
        case 0:
            fprintf(stderr, "The socket has been closed by the client!\n\n");
            close(clientSocket);
            return;
        default:
            printf("Message received: %s (%d bytes)\n\n", recvBuffer, bytesRead);
    }

    // Check for stop command
    if (strncmp(recvBuffer, "STOP", 4) == 0) {
        sprintf(sendBuffer, "Server stopping...\n");
        write(clientSocket, sendBuffer, strlen(sendBuffer));
        close(clientSocket);
        stop_server();
        return;
    }

    // Send data to the client
    sprintf(sendBuffer, "ok\n");
    bytesWritten = write(clientSocket, sendBuffer, strlen(sendBuffer));
    switch (bytesWritten) {
        case -1:
            perror("write");
            close(clientSocket);
            return;
        case 0:
            fprintf(stderr, "The socket has been closed by the client!\n\n");
            close(clientSocket);
            return;
        default:
            printf("Message %s sent (%d bytes)\n\n", sendBuffer, bytesWritten);
    }

    close(clientSocket);
}

void start_server() {
    struct sockaddr_in remoteAddress;
    socklen_t addressLength = sizeof(struct sockaddr_in);
    int clientSocket;

    signal(SIGINT, handle_sigint);

    listenSocket = create_listening_socket();

    // Connection wait loop
    while (server_should_run) {
        printf("Waiting for a connection request (exit with Ctrl-C or send 'STOP')\n\n");
        clientSocket = accept(listenSocket, (struct sockaddr *)&remoteAddress, &addressLength);
        if (clientSocket < 0) {
            if (server_should_run) perror("accept");
            continue;
        }
        handle_client(clientSocket);
    }
    close(listenSocket);
    printf("Server main loop exited.\n");
}