#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "client.h"

void help(char* command)
{
    printf("Usage: %s <ip_address> <mode>\n", command);
    printf("  <ip_address>: IP address of the other player\n");
    printf("  <mode>: 1 = first player, 2 = second player\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
        help(argv[0]);
        return 0;
    }

    if (argc != 3 && argc != 4) {
        help(argv[0]);
        return 1;
    }

    const char *ip_address = argv[1];
    int mode = atoi(argv[2]);

    if (mode == 1) {
        // First Player
        const char *msg = (argc == 4) ? argv[3] : "Hello world !\n";
        printf("Playing as the first player. Connecting to %s...\n", ip_address);
        send_infos(ip_address, msg);
        
    } else if (mode == 2) {
        // Second Player
        printf("Playing as the second player...\n");
        start_server();
    } else {
        printf("Invalid mode. Use 1 for first player, 2 for second player.\n");
        return 1;
    }

    return 0;
}