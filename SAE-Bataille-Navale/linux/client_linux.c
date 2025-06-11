#ifdef __linux__

#include "client_linux.h"

// Attempts to connect to a TCP server at server_ip and PORT.
// Returns socket descriptor (int) on success, -1 on failure.
int connect_to_server(const char* server_ip, bool debug) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket
    if (sock < 0) {
        perror("socket() failed");
        return -1;
    }
    if (debug) printf("Socket created successfully! (%d)\n", sock);

    memset(&server_addr, 0, sizeof(server_addr)); // Zero out struct
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // Set port

    // inet_pton: Convert IP string to binary
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid server IP address!\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Attempt to connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        if (debug) perror("connect() failed");
        close(sock);
        return -1;
    }

    if (debug) printf("Connected to server successfully!\n");
    return sock;
}

// Tries to connect repeatedly until successful. Sleeps 1 second between attempts.
int connection_loop(const char* server_ip, bool debug) {
    int sock = INVALID_SOCKET;
    while (sock == INVALID_SOCKET) {
        sock = connect_to_server(server_ip, debug);
        if (sock == INVALID_SOCKET) {
            if (debug) {
                fprintf(stderr, "Failed to connect to the server.\n");
                fprintf(stderr, "Trying to connect again in 1 second...\n");
            }
            sleep(1); // Sleep for 1 second
        }
    }
    return (int)sock;
}

// Sends a message to server. Exits on error or closed connection.
void send_message(int socket, const char* message, bool debug) {
    int bytes_sent = send(socket, message, strlen(message), 0);
    if (bytes_sent < 0) {
        perror("send() failed");
        close(socket);
        exit(-4);
    }
    else if (bytes_sent == 0) { // No bytes sent: server closed connection
        fprintf(stderr, "Socket closed by server while sending.\n");
        close(socket);
        exit(0);
    }
    else {
        if (debug) printf("Message '%s' sent successfully (%d bytes)\n", message, bytes_sent);
    }
}

// Receives message from server into buffer.
// Returns:
//   - number of bytes received (>0) on success
//   - 0 if connection closed by server
//   - -1 on socket error
//   - 404 if the message is "404"
int receive_message(int socket, char* buffer, int bufsize, bool debug) {
    int bytes_received = recv(socket, buffer, bufsize - 1, 0); // -1 to leave space for null terminator
    switch (bytes_received) {
    case -1:
        if (debug) perror("recv() failed");
        return -1;
    case 0:
        if (debug) fprintf(stderr, "Socket closed by server while receiving.\n");
        return 0;
    default:
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate received string
            if (debug) printf("Message received from server: %s (%d bytes)\n", buffer, bytes_received);
            if (strcmp(buffer, "404") == 0) return 404; // Magic value: "404" means special case
            return bytes_received;
        }
        else {
            if (debug) fprintf(stderr, "Unknown recv() result: %d\n", bytes_received);
            return -1;
        }
    }
}

// Closes a socket connection.
void close_connection(int socket, bool debug) {
    close(socket);
    if (debug) printf("Connection closed.\n");
}

#endif // __linux__