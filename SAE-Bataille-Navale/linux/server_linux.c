#ifdef __linux__

#include "server_linux.h"

// Globals for player sockets (used for cleanup/signaling on SIGINT)
int global_player1_fd = -1;
int global_player2_fd = -1;

// Prints error message, then exits program
void error_exit(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Creates, binds, and listens on a TCP socket on any interface at PORT.
// Returns the listening socket (int) or exits on error.
int create_listening_socket(void) {
    int server_fd;
    struct sockaddr_in addr;
    int optval = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
    if (server_fd < 0) {
        error_exit("socket");
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); // Allow address reuse
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    addr.sin_port = htons(PORT); // Set port

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        error_exit("bind");
    }
    if (listen(server_fd, 2) < 0) { // Listen for up to 2 connections
        close(server_fd);
        error_exit("listen");
    }
    return server_fd;
}

// Waits for a client to connect, prints their name on connection.
// Returns client socket (int) or exits on error.
int accept_client(int server_fd, const char* player_name, bool debug) {
    printf("Waiting for %s...\n", player_name);
    int client_fd = accept(server_fd, NULL, NULL); // Accept incoming connection
    if (client_fd < 0) {
        close(server_fd);
        error_exit("accept");
    }
    printf("%s connected!\n", player_name);
    return client_fd;
}

// Sends a message to a client. Prints debug info and closes socket on error.
void send_server_message(int client_fd, char* message, bool debug) {
    int sent = send(client_fd, message, strlen(message), 0);
    if (sent < 0) {
        if (debug) perror("Failed to send player number");
        close(client_fd);
    }
}

// Receives a message from the specified client socket.
// Returns the number of bytes received, 0 if client disconnected, or -1 on error.
// The buffer must be at least bufsize bytes.
int receive_client_message(int client_fd, char* buffer, int bufsize, int debug) {
    int n = recv(client_fd, buffer, bufsize - 1, 0); // Leave space for null terminator
    if (n < 0) {
        if (debug) perror("recv() failed");
        return -1;
    }
    if (n == 0) {
        if (debug) fprintf(stderr, "Client disconnected while receiving\n");
        printf("The other player has disconnected.");
        exit(0);
    }
    buffer[n] = '\0'; // Null-terminate for string use
    if (debug) printf("Received (%d bytes): %s\n", n, buffer);
    return n;
}

// Forwards a message from source_fd to dest_fd. Returns bytes relayed or -1 on error/disconnect.
int relay_message(int source_fd, int dest_fd) {
    char buffer[MSG_LEN];
    int n = recv(source_fd, buffer, MSG_LEN, 0); // Receive data
    if (n <= 0) return -1; // Error or disconnect
    int written = send(dest_fd, buffer, n, 0); // Forward to destination
    if (written < 0) {
        perror("send");
        return -1;
    }
    return n;
}

// Sends "404" to client and closes the socket (used to indicate disconnect).
void send_404_and_close(int fd) {
    const char msg[] = "404";
    if (fd >= 0) {
        send(fd, msg, strlen(msg), 0);
        close(fd);
    }
}

// Handler for SIGINT (Ctrl+C): notifies both clients and cleans up before exit.
void on_sigint(int signum) {
    printf("\nServer quitting. Notifying clients...\n");
    send_404_and_close(global_player1_fd);
    send_404_and_close(global_player2_fd);
    exit(0);
}

// Main relay loop: relays messages between two connected players using select().
// If a player disconnects, notifies the other and shuts down.
void relay_loop(int player1_fd, int player2_fd, bool debug) {
    fd_set readfds;
    int fds[2] = { player1_fd, player2_fd };
    global_player1_fd = player1_fd;
    global_player2_fd = player2_fd;

    while (1) {
        FD_ZERO(&readfds); // Clear set
        FD_SET(player1_fd, &readfds);
        FD_SET(player2_fd, &readfds);
        int maxfd = (player1_fd > player2_fd ? player1_fd : player2_fd) + 1;
        if (select(maxfd, &readfds, NULL, NULL, NULL) < 0) { // Wait for activity
            error_exit("select");
            break;
        }
        for (int i = 0; i < 2; ++i) {
            int from = fds[i];
            int to = fds[1 - i];
            if (FD_ISSET(from, &readfds)) { // Data ready on this socket?
                char buffer[MSG_LEN];
                int n = recv(from, buffer, MSG_LEN, 0);
                if (n <= 0) { // Player disconnected
                    printf("Player %d disconnected. Notifying the other player...\n", i + 1);
                    send_404_and_close(to);
                    close(from);
                    return;
                }
                if (debug) {
                    printf("Transferring %d bytes from player %d to player %d\n", n, i + 1, 2 - i);
                    printf("Content: \"%.*s\"\n", n, buffer);
                }
                int written = send(to, buffer, n, 0); // Relay to other player
                if (written < 0) {
                    perror("send");
                    return;
                }
            }
        }
    }
}

// Closes all sockets and cleans up resources.
void cleanup(int fd1, int fd2, int server_fd) {
    if (fd1 >= 0) close(fd1);
    if (fd2 >= 0) close(fd2);
    if (server_fd >= 0) close(server_fd);
}

#endif // __linux__