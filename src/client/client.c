#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

int main(int argc, char *argv[]) {
    int sockfd;
    char buf[MAXDATASIZE];
    struct hostent *host;
    struct sockaddr_in serv_addr;

    // Check if the user entered the server's hostname
    if (argc < 2) {
        fprintf(stderr, "Please enter the server's hostname!\n");
        exit(1);
    }

    // Resolve hostname
    if ((host = gethostbyname(argv[1])) == NULL) {
        perror("gethostbyname:");
        exit(1);
    }

    // Create socket and connect to server
    sockfd = create_socket_and_connect(host, &serv_addr);

    // Get files from server until the user types "fim"
    while (1) {
        printf("Type file name: ");
        scanf("%s", buf);

        if (strcmp(buf, "fim") == 0) break;

        send_file_name(sockfd, buf);
        receive_file(sockfd, buf);
    }

    close(sockfd);
    return 0;
}
