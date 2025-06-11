#ifdef __linux__

#include "client_linux.h"

int connect_to_server(const char* server_ip, bool debug) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket() failed");
        return -1;
    }
    if (debug) printf("Socket created successfully! (%d)\n", sock);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid server IP address!\n");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        if (debug) perror("connect() failed");
        close(sock);
        return -1;
    }

    if (debug) printf("Connected to server successfully!\n");
    return sock;
}

void send_message(int socket, const char* message, bool debug) {
    int bytes_sent = send(socket, message, strlen(message), 0);
    if (bytes_sent < 0) {
        perror("send() failed");
        close(socket);
        exit(-4);
    }
    else if (bytes_sent == 0) {
        fprintf(stderr, "Socket closed by server while sending.\n");
        close(socket);
        exit(0);
    }
    else {
        if (debug) printf("Message '%s' sent successfully (%d bytes)\n", message, bytes_sent);
    }
}

int receive_message(int socket, char* buffer, int bufsize, bool debug) {
    int bytes_received = recv(socket, buffer, bufsize - 1, 0);
    switch (bytes_received) {
    case -1:
        if (debug) perror("recv() failed");
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
    close(socket);
    if (debug) printf("Connection closed.\n");
}

#endif // __linux__