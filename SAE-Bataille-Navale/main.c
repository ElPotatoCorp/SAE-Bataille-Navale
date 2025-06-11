#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#include <signal.h>
#include "battleship.h"  

int server_fd = INVALID_SOCKET;
int player1_fd = INVALID_SOCKET;
int player2_fd = INVALID_SOCKET;

extern void clear();  

static void help(char* command) {
    printf(
        "Usage:\n"
        "  %s [--server | -s] [--debug]\n"
        "  %s [--player | -p] <ip_address> [--debug]\n"
        "  %s [--player | -p] [--host | -h] [--debug]\n"
        "\nOptions:\n"
        "  --server, -s                 Start in server mode (relay mode for two players)\n"
        "  --player, -p <ip>            Start in player mode and connect to server at <ip>\n"
        "  --player, -p --host, -h      Start in player mode and host the game\n"
        "  --debug                      Enable debug output\n"
        "  --help, -h                   Display this help message\n"
        "\nExamples:\n"
        "  %s --server --debug\n"
        "  %s -p 192.168.1.42\n"
        "\n",
        command, command, command, command, command
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

    clear();

    if (strcmp(argv[1], "--server") == 0 || strcmp(argv[1], "-s") == 0) {
        srand((unsigned int)time(NULL));
        char data[3];
		snprintf(data, sizeof(data), "%d%d", 1, rand() % 2 + 1);

#ifdef _WIN32
        initialize_winsock();
#endif

        server_fd = (int)create_listening_socket();
        player1_fd = (int)accept_client(server_fd, "Player 1", debug);
        player2_fd = (int)accept_client(server_fd, "Player 2", debug);

		send_message(player1_fd, data, debug);
        data[0] = 2 + '0';
        send_message(player2_fd, data, debug);
        relay_loop(player1_fd, player2_fd);

        cleanup(player1_fd, player2_fd, server_fd);
    }
    else if (strcmp(argv[1], "--player") == 0 || strcmp(argv[1], "-p") == 0) {
        char *ip_address = NULL;
		bool host_mode;
        if (strcmp(argv[2], "--host") == 0 || strcmp(argv[2], "-h") == 0) {
            host_mode = true;
            srand((unsigned int)time(NULL));
            char data[3];
            snprintf(data, sizeof(data), "%d%d", 2, rand() % 2 + 1);

#ifdef _WIN32
            initialize_winsock();
#endif

            server_fd = (int)create_listening_socket();
            player2_fd = (int)accept_client(server_fd, "Player 2", debug);

            send_message(player2_fd, data, debug);

            snprintf(ip_address, MSG_LEN, "%d %d", data[1], player2_fd); // Exploit the fact that ip_address is a set of characters to drop who's turn is it and the player's socket
        }
        else {
			host_mode = false;
            ip_address = argv[2];
        }
		bool restarted = false;
        while (true) {
            play(ip_address, restarted, host_mode, debug);
			printf("Game over. Press enter to play again or quit with (CTRL + C)\n");
            while (getchar() != '\n');
			restarted = true;
        }
    }

    return 0;
}