#ifdef _WIN32

#include "server_win.h"   

SOCKET global_player1_fd = INVALID_SOCKET;
SOCKET global_player2_fd = INVALID_SOCKET;

void error_exit(const char* msg) {
    fprintf(stderr, "%s: %d\n", msg, WSAGetLastError());
    WSACleanup();
    exit(EXIT_FAILURE);
}

void initialize_winsock(void) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }
}

SOCKET create_listening_socket(void) {
    SOCKET server_fd;
    struct sockaddr_in addr;
    int optval = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        error_exit("socket");
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(server_fd);
        error_exit("bind");
    }
    if (listen(server_fd, 2) == SOCKET_ERROR) {
        closesocket(server_fd);
        error_exit("listen");
    }
    return server_fd;
}

SOCKET accept_client(SOCKET server_fd, const char* player_name, bool debug) {
    printf("Waiting for %s...\n", player_name);
    SOCKET client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == INVALID_SOCKET) {
        closesocket(server_fd);
        error_exit("accept");
    }
    printf("%s connected!\n", player_name);
    return client_fd;
}

void send_server_message(SOCKET client_fd, char *message, bool debug) {
    int sent = send(client_fd, message, (int)strlen(message), 0);
    if (sent == SOCKET_ERROR) {
        if (debug) fprintf(stderr, "Failed to send player number: %d\n", WSAGetLastError());
        closesocket(client_fd);
    }
}

int relay_message(SOCKET source_fd, SOCKET dest_fd) {
    char buffer[MSG_LEN];
    int n = recv(source_fd, buffer, MSG_LEN, 0);
    if (n <= 0) return -1;
    int written = send(dest_fd, buffer, n, 0);
    if (written == SOCKET_ERROR) {
        error_exit("send");
        return -1;
    }
    return n;
}

void send_404_and_close(SOCKET fd) {
    const char msg[] = "404";
    if (fd != INVALID_SOCKET) {
        send(fd, msg, (int)strlen(msg), 0);
        closesocket(fd);
    }
}

void on_sigint(int signum) {
    printf("\nServer quitting. Notifying clients...\n");
    send_404_and_close(global_player1_fd);
    send_404_and_close(global_player2_fd);
    WSACleanup();
    exit(0);
}

void relay_loop(SOCKET player1_fd, SOCKET player2_fd) {
    fd_set readfds;
    SOCKET fds[2] = { player1_fd, player2_fd };
    global_player1_fd = player1_fd;
    global_player2_fd = player2_fd;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(player1_fd, &readfds);
        FD_SET(player2_fd, &readfds);
        if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) {
            error_exit("select");
            break;
        }
        for (int i = 0; i < 2; ++i) {
            SOCKET from = fds[i];
            SOCKET to = fds[1 - i];
            if (FD_ISSET(from, &readfds)) {
                char buffer[MSG_LEN];
                int n = recv(from, buffer, MSG_LEN, 0);
                if (n <= 0) {
                    printf("Player %d disconnected. Notifying the other player...\n", i + 1);
                    send_404_and_close(to);
                    closesocket(from);
                    return;
                }
                int written = send(to, buffer, n, 0);
                if (written == SOCKET_ERROR) {
                    error_exit("send");
                    return;
                }
            }
        }
    }
}

void cleanup(SOCKET fd1, SOCKET fd2, SOCKET server_fd) {
    closesocket(fd1);
    closesocket(fd2);
    closesocket(server_fd);
    WSACleanup();
}

#endif // _WIN32