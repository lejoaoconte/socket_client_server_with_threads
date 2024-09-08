#include "network.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../server.h"

int create_socket() {
    int sockfd;
    // Create the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket:");
        exit(1);
    }

    int on = 1;
    // Set the socket option
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    return sockfd;
}

void bind_socket(int sockfd, struct sockaddr_in *server_sockaddr) {
    // Bind the socket to the server address
    if ((bind(sockfd, (struct sockaddr *)server_sockaddr,
              sizeof(struct sockaddr))) == -1) {
        perror("bind:");
        exit(1);
    }
}

void listen_for_connections(int sockfd) {
    // Start listening for incoming connections
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen:");
        exit(1);
    }
    printf("Server started and listening...\n");
}

int accept_client(int sockfd, struct sockaddr_in *client_sockaddr) {
    int sin_size = sizeof(struct sockaddr_in);
    int client_fd;
    // Accept an incoming client connection
    if ((client_fd = accept(sockfd, (struct sockaddr *)client_sockaddr,
                            (socklen_t *)&sin_size)) == -1) {
        perror("accept:");
    }
    return client_fd;
}
