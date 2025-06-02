#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "client.h"

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        printf("Usage: %s <ip_address> <mode>\n", argv[0]);
        printf("  <ip_address>: For client mode, the server's IP address. For server mode, can be any string.\n");
        printf("  <mode>: 0 = server, 1 = client\n");
        return 1;
    }

    const char *ip_address = argv[1];
    int mode = atoi(argv[2]);

    if (mode == 0) {
        // Server mode
        printf("Starting in SERVER mode...\n");
        start_server();
    } else if (mode == 1) {
        // Client mode
        const char *msg = (argc > 3) ? argv[3] : "Hello world !\n";
        printf("Starting in CLIENT mode. Connecting to %s...\n", ip_address);
        send_infos(ip_address, msg);
    } else {
        printf("Invalid mode. Use 0 for server, 1 for client.\n");
        return 1;
    }

    return 0;
}