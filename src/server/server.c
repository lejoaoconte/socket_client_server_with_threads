#include "server.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./utils/network.h"
#include "./utils/thread.h"

int main() {
    struct sockaddr_in server_sockaddr, client_sockaddr;
    int sockfd;
    pthread_t thread_id;

    // Create and configure the server socket
    sockfd = create_socket();

    // Set up the server address
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(SERVPORT);
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_sockaddr.sin_zero), 0, 8);

    // Bind the socket to the server address
    bind_socket(sockfd, &server_sockaddr);

    // Start listening for incoming connections
    listen_for_connections(sockfd);

    while (1) {
        // Accept an incoming client connection
        int client_fd = accept_client(sockfd, &client_sockaddr);

        if (client_fd == -1) {
            continue;  // If accepting failed, skip this iteration
        }

        // Print information about the client
        printf("Accepted connection from client: %s, port: %d\n",
               inet_ntoa(client_sockaddr.sin_addr),
               ntohs(client_sockaddr.sin_port));

        // Create a thread to handle the client's request
        if (pthread_create(&thread_id, NULL, process_client_thread,
                           &client_fd) != 0) {
            perror("pthread_create:");
            close(client_fd);
            continue;
        }

        // Detach the thread to let it clean up after itself
        pthread_detach(thread_id);
    }

    // Close the server socket
    close(sockfd);
    return 0;
}
