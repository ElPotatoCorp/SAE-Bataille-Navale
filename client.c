#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// Connects to the server at the given IP address, returns the socket descriptor.
int connect_to_server(const char *server_ip) {
    int sock;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(-1);
    }
    printf("Socket created successfully! (%d)\n", sock);

    memset(&server_addr, 0x00, addr_len);
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_aton(server_ip, &server_addr.sin_addr) == 0) {
        fprintf(stderr, "Invalid server IP address!\n");
        close(sock);
        exit(-2);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, addr_len) == -1) {
        perror("connect");
        close(sock);
        exit(-3);
    }
    printf("Connected to server successfully!\n");
    return sock;
}


// Sends a message to the server.
void send_message(int socket, const char *message) {
    int bytes_sent = write(socket, message, strlen(message));
    switch (bytes_sent) {
        case -1:
            perror("write");
            close(socket);
            exit(-4);
        case 0:
            fprintf(stderr, "Socket closed by server while sending.\n");
            close(socket);
            exit(0);
        default:
            printf("Message '%s' sent successfully (%d bytes)\n", message, bytes_sent);
    }
}

// Receives a message from the server.
void receive_message(int socket, char *buffer, int bufsize) {
    int bytes_received = read(socket, buffer, bufsize);
    switch (bytes_received) {
        case -1:
            perror("read");
            close(socket);
            exit(-5);
        case 0:
            fprintf(stderr, "Socket closed by server while receiving.\n");
            close(socket);
            exit(0);
        default:
            buffer[bytes_received] = '\0';
            printf("Message received from server: %s (%d bytes)\n", buffer, bytes_received);
    }
}

// Closes the socket connection.
void close_connection(int socket) {
    close(socket);
    printf("Connection closed.\n");
}

void send_infos(const char *ip_address, const char *message) {
    int socket;
    char recv_buffer[MSG_LEN];

    socket = connect_to_server(ip_address);

    send_message(socket, message);
    receive_message(socket, recv_buffer, MSG_LEN - 1);

    close_connection(socket);
}

void try_send_infos(const char *ip_address, const char *message) {
    int socket;
    char recv_buffer[MSG_LEN];

    while (1) {
        socket = connect_to_server(ip_address);
        if (socket >= 0) {
            break;
        }
        printf("Connection failed, retrying in 1 second...\n");
        sleep(1);
    }

    send_message(socket, message);
    receive_message(socket, recv_buffer, MSG_LEN - 1);

    close_connection(socket);
}