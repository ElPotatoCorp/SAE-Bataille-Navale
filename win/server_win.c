#include "server_win.h"

int listenSocket = INVALID_SOCKET;

static void winsock_init(void) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

static void winsock_cleanup(void) {
    WSACleanup();
}

int create_listening_socket(void) {
    winsock_init();
    SOCKET sock;
    struct sockaddr_in localAddress;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
        winsock_cleanup();
        exit(-1);
    }

    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0) {
        fprintf(stderr, "setsockopt() failed: %d\n", WSAGetLastError());
        closesocket(sock);
        winsock_cleanup();
        exit(-1);
    }

    memset(&localAddress, 0, sizeof(localAddress));
    localAddress.sin_family = AF_INET;
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddress.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&localAddress, sizeof(localAddress)) < 0) {
        fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
        closesocket(sock);
        winsock_cleanup();
        exit(-2);
    }

    if (listen(sock, 5) < 0) {
        fprintf(stderr, "listen() failed: %d\n", WSAGetLastError());
        closesocket(sock);
        winsock_cleanup();
        exit(-3);
    }

    return (int)sock;
}

const char *recv_infos(void) {
    struct sockaddr_in remoteAddress;
    int clientSocket;
    char *recvBuffer = (char*)malloc(MSG_LEN * sizeof(char));
    int addressLength = sizeof(remoteAddress);
    if (!recvBuffer) {
        fprintf(stderr, "malloc() failed\n");
        return NULL;
    }

    listenSocket = create_listening_socket();

    clientSocket = accept((SOCKET)listenSocket, (struct sockaddr *)&remoteAddress, &addressLength);
    if (clientSocket < 0) {
        fprintf(stderr, "accept() failed: %d\n", WSAGetLastError());
        closesocket((SOCKET)listenSocket);
        free(recvBuffer);
        winsock_cleanup();
        return NULL;
    }

    memset(recvBuffer, 0, MSG_LEN * sizeof(char));

    int bytesRead = recv((SOCKET)clientSocket, recvBuffer, MSG_LEN - 1, 0);
    if (bytesRead > 0) {
        recvBuffer[bytesRead] = '\0';
        const char *feedback = "OK";
        if (send((SOCKET)clientSocket, feedback, (int)strlen(feedback), 0) < 0) {
            fprintf(stderr, "send(feedback) failed: %d\n", WSAGetLastError());
        }
    } else if (bytesRead == 0) {
        printf("The socket has been closed by the client!\n");
        free(recvBuffer);
        recvBuffer = NULL;
    } else {
        fprintf(stderr, "recv() failed: %d\n", WSAGetLastError());
        free(recvBuffer);
        recvBuffer = NULL;
    }

    closesocket((SOCKET)clientSocket);
    closesocket((SOCKET)listenSocket);
    listenSocket = INVALID_SOCKET;
    winsock_cleanup();

    return recvBuffer;
}