#ifdef __linux__

#include "server_linux.h"

int global_player1_fd = -1;
int global_player2_fd = -1;

void error_exit(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int create_listening_socket(void) {
    int server_fd;
    struct sockaddr_in addr;
    int optval = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        error_exit("socket");
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        error_exit("bind");
    }
    if (listen(server_fd, 2) < 0) {
        close(server_fd);
        error_exit("listen");
    }
    return server_fd;
}

int accept_client(int server_fd, const char* player_name, bool debug) {
    printf("Waiting for %s...\n", player_name);
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        close(server_fd);
        error_exit("accept");
    }
    printf("%s connected!\n", player_name);
    return client_fd;
}

void send_server_message(int client_fd, char* message, bool debug) {
    int sent = send(client_fd, message, strlen(message), 0);
    if (sent < 0) {
        if (debug) perror("Failed to send player number");
        close(client_fd);
    }
}

int relay_message(int source_fd, int dest_fd) {
    char buffer[MSG_LEN];
    int n = recv(source_fd, buffer, MSG_LEN, 0);
    if (n <= 0) return -1;
    int written = send(dest_fd, buffer, n, 0);
    if (written < 0) {
        perror("send");
        return -1;
    }
    return n;
}

void send_404_and_close(int fd) {
    const char msg[] = "404";
    if (fd >= 0) {
        send(fd, msg, strlen(msg), 0);
        close(fd);
    }
}

void on_sigint(int signum) {
    printf("\nServer quitting. Notifying clients...\n");
    send_404_and_close(global_player1_fd);
    send_404_and_close(global_player2_fd);
    exit(0);
}

void relay_loop(int player1_fd, int player2_fd) {
    fd_set readfds;
    int fds[2] = { player1_fd, player2_fd };
    global_player1_fd = player1_fd;
    global_player2_fd = player2_fd;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(player1_fd, &readfds);
        FD_SET(player2_fd, &readfds);
        int maxfd = (player1_fd > player2_fd ? player1_fd : player2_fd) + 1;
        if (select(maxfd, &readfds, NULL, NULL, NULL) < 0) {
            error_exit("select");
            break;
        }
        for (int i = 0; i < 2; ++i) {
            int from = fds[i];
            int to = fds[1 - i];
            if (FD_ISSET(from, &readfds)) {
                char buffer[MSG_LEN];
                int n = recv(from, buffer, MSG_LEN, 0);
                if (n <= 0) {
                    printf("Player %d disconnected. Notifying the other player...\n", i + 1);
                    send_404_and_close(to);
                    close(from);
                    return;
                }
                int written = send(to, buffer, n, 0);
                if (written < 0) {
                    perror("send");
                    return;
                }
            }
        }
    }
}

void cleanup(int fd1, int fd2, int server_fd) {
    if (fd1 >= 0) close(fd1);
    if (fd2 >= 0) close(fd2);
    if (server_fd >= 0) close(server_fd);
}

#endif // __linux__