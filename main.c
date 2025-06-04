#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "battleship.h"

void help(char* command)
{
    printf("Usage: %s <ip_address> <mode> --debug(optional)\n", command);
    printf("  <ip_address>: IP address of the other player\n");
    printf("  <mode>: 1 = first player, 2 = second player\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
        help(argv[0]);
        return 0;
    }

    if (argc < 3 && argc > 4) {
        help(argv[0]);
        return 1;
    }

    const char *ip_address = argv[1];
    int player = atoi(argv[2]);

    if (player != 1 && player != 2) {
        printf("Invalid mode. Use 1 for first player, 2 for second player.\n");
        return 1;
    }

    srand(time(NULL));
    system("clear"); 

    (argc == 4 && strcmp(argv[3], "--debug") == 0) ? play(player, ip_address, true) : play(player, ip_address, false);
    return 0;
}