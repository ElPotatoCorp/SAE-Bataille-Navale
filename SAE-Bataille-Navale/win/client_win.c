#include "client_win.h"

static void winsock_init(void) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

static void winsock_cleanup(void) {
    WSACleanup();
}

int connect_to_server(const char* server_ip, bool debug) {
    winsock_init();

    SOCKET sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed with error: %d\n", WSAGetLastError());
        winsock_cleanup();
        return -1;
    }
    if (debug) printf("Socket created successfully! (%llu)\n", (unsigned long long)sock);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid server IP address!\n");
        closesocket(sock);
        winsock_cleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        if (debug) fprintf(stderr, "connect() failed: %d\n", WSAGetLastError());
        closesocket(sock);
        winsock_cleanup();
        return -1;
    }

    if (debug) printf("Connected to server successfully!\n");
    return (int)sock;
}

void send_message(int socket, const char* message, bool debug) {
    int bytes_sent = send((SOCKET)socket, message, (int)strlen(message), 0);
    if (bytes_sent == SOCKET_ERROR) {
        fprintf(stderr, "send() failed: %d\n", WSAGetLastError());
        closesocket((SOCKET)socket);
        winsock_cleanup();
        exit(-4);
    }
    else if (bytes_sent == 0) {
        fprintf(stderr, "Socket closed by server while sending.\n");
        closesocket((SOCKET)socket);
        winsock_cleanup();
        exit(0);
    }
    else {
        if (debug) printf("Message '%s' sent successfully (%d bytes)\n", message, bytes_sent);
    }
}

int receive_message(int socket, char* buffer, int bufsize, bool debug) {
    int bytes_received = recv((SOCKET)socket, buffer, bufsize - 1, 0);
    switch (bytes_received) {
    case SOCKET_ERROR:
        if (debug) fprintf(stderr, "recv() failed: %d\n", WSAGetLastError());
        return -1;
    case 0:
        if (debug) fprintf(stderr, "Socket closed by server while receiving.\n");
        return 0;
    default:
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            if (debug) printf("Message received from server: %s (%d bytes)\n", buffer, bytes_received);
            if (strcmp(buffer, "404") == 0) return 404;
            return bytes_received;
        }
        else {
            if (debug) fprintf(stderr, "Unknown recv() result: %d\n", bytes_received);
            return -1;
        }
    }
}

void close_connection(int socket, bool debug) {
    closesocket((SOCKET)socket);
    if (debug) printf("Connection closed.\n");
    winsock_cleanup();
}
