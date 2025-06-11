#ifdef _WIN32

#include "server_win.h"   // Custom header for Windows server-side code

SOCKET global_player1_fd = INVALID_SOCKET; // Global: used for cleanup/signaling on SIGINT
SOCKET global_player2_fd = INVALID_SOCKET;

// Prints error message, cleans up Winsock, and exits program
void error_exit(const char* msg) {
    fprintf(stderr, "%s: %d\n", msg, WSAGetLastError());
    WSACleanup();
    exit(EXIT_FAILURE);
}

// Initializes Winsock (must be called before using sockets).
void initialize_winsock(void) {
    WSADATA wsa; // Holds info about Winsock implementation
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { // Request Winsock 2.2
        fprintf(stderr, "WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }
}

// Creates, binds, and listens on a TCP socket on any interface at PORT.
// Returns the listening socket (SOCKET) or exits on error.
SOCKET create_listening_socket(void) {
    SOCKET server_fd;
    struct sockaddr_in addr;
    int optval = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
    if (server_fd == INVALID_SOCKET) {
        error_exit("socket");
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)); // Allow address reuse
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    addr.sin_port = htons(PORT); // Set port

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(server_fd);
        error_exit("bind");
    }
    if (listen(server_fd, 2) == SOCKET_ERROR) { // Listen for up to 2 connections
        closesocket(server_fd);
        error_exit("listen");
    }
    return server_fd;
}

// Waits for a client to connect, prints their name on connection.
// Returns client socket (SOCKET) or exits on error.
SOCKET accept_client(SOCKET server_fd, const char* player_name, bool debug) {
    printf("Waiting for %s...\n", player_name);
    SOCKET client_fd = accept(server_fd, NULL, NULL); // Accept incoming connection
    if (client_fd == INVALID_SOCKET) {
        closesocket(server_fd);
        error_exit("accept");
    }
    printf("%s connected!\n", player_name);
    return client_fd;
}

// Sends a message to a client. Prints debug info and closes socket on error.
void send_server_message(SOCKET client_fd, char* message, bool debug) {
    int sent = send(client_fd, message, (int)strlen(message), 0);
    if (sent == SOCKET_ERROR) {
        if (debug) fprintf(stderr, "Failed to send player number: %d\n", WSAGetLastError());
        closesocket(client_fd);
    }
}

// Forwards a message from source_fd to dest_fd. Returns bytes relayed or -1 on error/disconnect.
int relay_message(SOCKET source_fd, SOCKET dest_fd) {
    char buffer[MSG_LEN];
    int n = recv(source_fd, buffer, MSG_LEN, 0); // Receive data
    if (n <= 0) return -1; // Error or disconnect
    int written = send(dest_fd, buffer, n, 0); // Forward to destination
    if (written == SOCKET_ERROR) {
        error_exit("send");
        return -1;
    }
    return n;
}

// Sends "404" to client and closes the socket (used to indicate disconnect).
void send_404_and_close(SOCKET fd) {
    const char msg[] = "404";
    if (fd != INVALID_SOCKET) {
        send(fd, msg, (int)strlen(msg), 0);
        closesocket(fd);
    }
}

// Handler for SIGINT (Ctrl+C): notifies both clients and cleans up before exit.
void on_sigint(int signum) {
    printf("\nServer quitting. Notifying clients...\n");
    send_404_and_close(global_player1_fd);
    send_404_and_close(global_player2_fd);
    WSACleanup();
    exit(0);
}

// Main relay loop: relays messages between two connected players using select().
// If a player disconnects, notifies the other and shuts down.
void relay_loop(SOCKET player1_fd, SOCKET player2_fd) {
    fd_set readfds;
    SOCKET fds[2] = { player1_fd, player2_fd };
    global_player1_fd = player1_fd;
    global_player2_fd = player2_fd;

    while (true) {
        FD_ZERO(&readfds); // Clear set
        FD_SET(player1_fd, &readfds);
        FD_SET(player2_fd, &readfds);
        if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) { // Wait for activity
            error_exit("select");
            break;
        }
        for (int i = 0; i < 2; ++i) {
            SOCKET from = fds[i];
            SOCKET to = fds[1 - i];
            if (FD_ISSET(from, &readfds)) { // Data ready on this socket?
                char buffer[MSG_LEN];
                int n = recv(from, buffer, MSG_LEN, 0);
                if (n <= 0) { // Player disconnected
                    printf("Player %d disconnected. Notifying the other player...\n", i + 1);
                    send_404_and_close(to);
                    closesocket(from);
                    return;
                }
                int written = send(to, buffer, n, 0); // Relay to other player
                if (written == SOCKET_ERROR) {
                    error_exit("send");
                    return;
                }
            }
        }
    }
}

// Closes all sockets and cleans up Winsock resources.
void cleanup(SOCKET fd1, SOCKET fd2, SOCKET server_fd) {
    closesocket(fd1);
    closesocket(fd2);
    closesocket(server_fd);
    WSACleanup();
}

#endif // _WIN32