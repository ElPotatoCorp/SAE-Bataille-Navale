#include "server.h"

int listenSocket = -1;

int create_listening_socket() {
    int sock;
    struct sockaddr_in localAddress;
    socklen_t addressLength = sizeof(struct sockaddr_in);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(-1);
    }
    
    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        close(sock);
        exit(-1);
    }

    memset(&localAddress, 0x00, addressLength);
    localAddress.sin_family = PF_INET;
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddress.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&localAddress, addressLength) < 0) {
        perror("bind");
        exit(-2);
    }

    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(-3);
    }

    return sock;
}

const char *recv_infos() {
    struct sockaddr_in remoteAddress;
    socklen_t addressLength = sizeof(struct sockaddr_in);
    int clientSocket;
    char* recvBuffer = malloc(MSG_LEN * sizeof(char));
    if (!recvBuffer) {
        perror("malloc");
        return NULL;
    }

    listenSocket = create_listening_socket();

    printf("Waiting for other player...\n");

    clientSocket = accept(listenSocket, (struct sockaddr *)&remoteAddress, &addressLength);
    if (clientSocket < 0) {
        perror("accept");
        close(listenSocket);
        free(recvBuffer);
        return NULL;
    }

    memset(recvBuffer, 0x00, MSG_LEN * sizeof(char));

    int bytesRead = read(clientSocket, recvBuffer, MSG_LEN - 1);
    if (bytesRead > 0) {
        recvBuffer[bytesRead] = '\0';
    } else if (bytesRead == 0) {
        printf("The socket has been closed by the client!\n");
        free(recvBuffer);
        recvBuffer = NULL;
    } else {
        perror("read");
        free(recvBuffer);
        recvBuffer = NULL;
    }

    close(clientSocket);
    close(listenSocket);
    listenSocket = -1;

    return recvBuffer;
}