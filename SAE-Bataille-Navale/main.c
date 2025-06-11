#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#include <signal.h>
#include "battleship.h"  

extern void clear();  

static void help(char* command) {
    printf(
        "Usage:\n"
        "  %s [--server | -s] [--debug]\n"
        "  %s [--player | -p] <ip_address> [--debug]\n"
        "\nOptions:\n"
        "  --server, -s            Start in server mode (relay mode for two players)\n"
        "  --player, -p <ip>       Start in player mode and connect to server at <ip>\n"
        "  --debug                 Enable debug output\n"
        "  --help, -h              Display this help message\n"
        "\nExamples:\n"
        "  %s --server --debug\n"
        "  %s -p 192.168.1.42\n"
        "\n",
        command, command, command, command
    );
}

int main(int argc, char *argv[]) {  
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {  
        help(argv[0]);  
        return 0;  
    }  

    if (argc < 2 && argc > 4) {  
        help(argv[0]);  
        return 1;  
    }  

    bool debug = (strcmp(argv[argc - 1], "--debug") == 0);

    signal(SIGINT, on_sigint);

    if (strcmp(argv[1], "--server") == 0 || strcmp(argv[1], "-s") == 0) {
        srand((unsigned int)time(NULL)); // Explicit cast to unsigned int to resolve warning  
        char data[3];
		snprintf(data, sizeof(data), "%d%d", 1, rand() % 2 + 1);

        initialize_winsock();
        SOCKET server_fd = create_listening_socket();
        SOCKET player1_fd = accept_client(server_fd, "Player 1", debug);
        SOCKET player2_fd = accept_client(server_fd, "Player 2", debug);

		send_message(player1_fd, data, debug);
        data[0] = 2 + '0';
        send_message(player2_fd, data, debug);
        relay_loop(player1_fd, player2_fd);

        cleanup(player1_fd, player2_fd, server_fd);
    }
    else if (strcmp(argv[1], "--player") == 0 || strcmp(argv[1], "-p") == 0) {
        char *ip_address = argv[2];

        clear();

        play(ip_address, debug);
    }

    return 0;
}