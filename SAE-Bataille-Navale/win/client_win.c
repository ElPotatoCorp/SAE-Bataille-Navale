#ifdef _WIN32

#include "client_win.h"

// Note: WSAStartup must be called before using any Winsock functions.
static void winsock_init(void) {
    WSADATA wsa; // WSADATA: struct that receives details about the Windows Sockets implementation
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { // Init Winsock 2.2
        fprintf(stderr, "WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

// Note: WSACleanup should be called to release Winsock resources.
static void winsock_cleanup(void) {
    WSACleanup();
}

// Attempts to connect to a TCP server at server_ip and PORT.
// Returns socket descriptor (as int) on success, -1 on failure.
int connect_to_server(const char* server_ip, bool debug) {
    winsock_init(); // Must be called before using sockets (Windows only).

    SOCKET sock; // SOCKET: unsigned integral type for sockets on Windows.
    struct sockaddr_in server_addr; // sockaddr_in: IPv4 address structure.

    sock = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket.
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed with error: %d\n", WSAGetLastError());
        winsock_cleanup();
        return -1;
    }
    if (debug) printf("Socket created successfully! (%llu)\n", (unsigned long long)sock);

    memset(&server_addr, 0, sizeof(server_addr)); // Zero out struct.
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // htons: host to network byte order.

    // inet_pton: Convert IP string to binary.
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid server IP address!\n");
        closesocket(sock); // Use closesocket on Windows.
        winsock_cleanup();
		exit(EXIT_FAILURE);
    }

    // Attempt to connect to the server.
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        if (debug) fprintf(stderr, "connect() failed: %d\n", WSAGetLastError());
        closesocket(sock);
        winsock_cleanup();
        return -1;
    }

    if (debug) printf("Connected to server successfully!\n");
    // Note: SOCKET and int are not guaranteed to be same size, but cast is used here for compatibility.
    return (int)sock;
}

// Tries to connect repeatedly until successful. Sleeps 1 second between attempts.
int connection_loop(const char* server_ip, bool debug) {
    SOCKET sock = INVALID_SOCKET;
    while (sock == INVALID_SOCKET) {
        sock = connect_to_server(server_ip, debug);
        if (sock == INVALID_SOCKET) {
            if (debug) {
                fprintf(stderr, "Failed to connect to the server.\n");
                fprintf(stderr, "Trying to connect again in 1 second...\n");
            }
            Sleep(1000);
        }
    }
    return (int)sock;
}

// Sends a message to server. Exits on error or closed connection.
void send_message(int socket, const char* message, bool debug) {
    int bytes_sent = send((SOCKET)socket, message, (int)strlen(message), 0);
    if (bytes_sent == SOCKET_ERROR) {
        fprintf(stderr, "send() failed: %d\n", WSAGetLastError());
        closesocket((SOCKET)socket);
        winsock_cleanup();
        exit(-4);
    }
    else if (bytes_sent == 0) { // No bytes sent: server closed connection.
        fprintf(stderr, "Socket closed by server while sending.\n");
        closesocket((SOCKET)socket);
        winsock_cleanup();
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
    int bytes_received = recv((SOCKET)socket, buffer, bufsize - 1, 0); // -1 to leave space for null terminator.
    switch (bytes_received) {
    case SOCKET_ERROR:
        if (debug) fprintf(stderr, "recv() failed: %d\n", WSAGetLastError());
        return -1;
    case 0:
        if (debug) fprintf(stderr, "Socket closed by server while receiving.\n");
        return 0;
    default:
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate received string.
            if (debug) printf("Message received from server: %s (%d bytes)\n", buffer, bytes_received);
            if (strcmp(buffer, "404") == 0) return 404; // Magic value: "404" means special case.
            return bytes_received;
        }
        else {
            if (debug) fprintf(stderr, "Unknown recv() result: %d\n", bytes_received);
            return -1;
        }
    }
}

// Closes a socket and cleans up Winsock.
void close_connection(int socket, bool debug) {
    closesocket((SOCKET)socket); // closesocket is required for Windows sockets.
    if (debug) printf("Connection closed.\n");
    winsock_cleanup();
}

#endif // _WIN32