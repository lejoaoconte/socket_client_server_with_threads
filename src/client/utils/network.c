#include "network.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../client.h"

int create_socket_and_connect(struct hostent* host,
                              struct sockaddr_in* serv_addr) {
    int sockfd;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket:");
        exit(1);
    }

    // Set server address
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(SEVPORT);
    serv_addr->sin_addr = *((struct in_addr*)host->h_addr_list[0]);
    bzero(&(serv_addr->sin_zero), 8);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)serv_addr, sizeof(struct sockaddr)) ==
        -1) {
        perror("connect:");
        exit(1);
    }

    return sockfd;
}
