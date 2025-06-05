#ifdef _WIN32

#include "client_win.h"

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

int connect_to_server(const char *server_ip, bool debug) {
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
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid server IP address!\n");
        closesocket(sock);
        winsock_cleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        if (debug) fprintf(stderr, "connect() failed: %d\n", WSAGetLastError());
        closesocket(sock);
        winsock_cleanup();
        return -1;
    }

    if (debug) printf("Connected to server successfully!\n");
    return (int)sock;
}

void send_message(int socket, const char *message, bool debug) {
    int bytes_sent = send((SOCKET)socket, message, (int)strlen(message), 0);
    if (bytes_sent == SOCKET_ERROR) {
        fprintf(stderr, "send() failed: %d\n", WSAGetLastError());
        closesocket((SOCKET)socket);
        winsock_cleanup();
        exit(-4);
    } else if (bytes_sent == 0) {
        fprintf(stderr, "Socket closed by server while sending.\n");
        closesocket((SOCKET)socket);
        winsock_cleanup();
        exit(0);
    } else {
        if (debug) printf("Message '%s' sent successfully (%d bytes)\n", message, bytes_sent);
    }
}

int receive_message(int socket, char *buffer, int bufsize, bool debug) {
    int bytes_received = recv((SOCKET)socket, buffer, bufsize, 0);
    if (bytes_received == SOCKET_ERROR) {
        fprintf(stderr, "recv() failed: %d\n", WSAGetLastError());
        closesocket((SOCKET)socket);
        winsock_cleanup();
        return -1;
    } else if (bytes_received == 0) {
        if (debug) fprintf(stderr, "Socket closed by server while receiving.\n");
        closesocket((SOCKET)socket);
        winsock_cleanup();
        return 0;
    } else {
        buffer[bytes_received] = '\0';
        if (debug) printf("Message received from server: %s (%d bytes)\n", buffer, bytes_received);
        return 1;
    }
}

void close_connection(int socket, bool debug) {
    closesocket((SOCKET)socket);
    if (debug) printf("Connection closed.\n");
    winsock_cleanup();
}

void send_infos(const char *ip_address, const char *message, bool debug) {
    int socket;
    char recv_buffer[MSG_LEN];

    socket = connect_to_server(ip_address, debug);
    send_message(socket, message, debug);
    receive_message(socket, recv_buffer, MSG_LEN - 1, debug);
    close_connection(socket, debug);
}

bool try_send_infos(const char *ip_address, const char *message, bool debug) {
    int socket;
    char recv_buffer[MSG_LEN];

    while (1) {
        socket = connect_to_server(ip_address, debug);
        if (socket >= 0) break;
        if (debug) printf("Connection failed, retrying in 1 second...\n");
        Sleep(1000);
    }

    bool success;
    send_message(socket, message, debug);
    if (receive_message(socket, recv_buffer, MSG_LEN - 1, debug) == -1) success = false;
    else success = true;

    close_connection(socket, debug);
    return success;
}

#endif