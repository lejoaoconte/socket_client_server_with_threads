#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVPORT 3333
#define BACKLOG 10
#define MAX_CONNECTED_NO 10
#define MAXDATASIZE (5 * 1024)

// Function process_client_thread prototypes
void *process_client_thread(void *);

int main() {
    struct sockaddr_in server_sockaddr, client_sockaddr;
    int sin_size;
    int sockfd;
    pthread_t thread_id;

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket:");
        exit(1);
    }

    int on;
    on = 1;
    // Set the socket options
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(SERVPORT);
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_sockaddr.sin_zero), 0, 8);

    // Bind the socket
    if ((bind(sockfd, (struct sockaddr *)&server_sockaddr,
              sizeof(struct sockaddr))) == -1) {
        perror("bind:");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen:");
        exit(1);
    }
    printf("Start listen..... \n");

    // Accept incoming connections
    while (1) {
        int client_fd;
        sin_size = sizeof(struct sockaddr);
        struct sockaddr_in peeraddr;
        int len = sizeof(peeraddr);
        char server_ip[20];

        // Accept the connection from the client
        if ((client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr,
                                &sin_size)) == -1) {
            perror("accept:");
            continue;
        }
        // Print the client address and port number
        printf("accept socket..... Client address: %s Port: %d\n",
               inet_ntoa(client_sockaddr.sin_addr),
               ntohs(client_sockaddr.sin_port));
        getsockname(client_fd, (struct sockaddr *)&peeraddr, &len);
        inet_ntop(AF_INET, &peeraddr.sin_addr, server_ip, sizeof(server_ip));
        printf("accept socket..... Server address: %s \n", server_ip);

        // Create a thread to process the client request
        if (pthread_create(&thread_id, NULL, process_client_thread,
                           &client_fd) != 0) {
            perror("pthread_create:");
            close(client_fd);
            continue;
        }
        // Detach the thread
        pthread_detach(thread_id);
    }
    close(sockfd);
}

// Function to process client requests
void *process_client_thread(void *arg) {
    int client_fd = *(int *)arg;
    char buf[MAXDATASIZE];
    int recvbytes, sendbytes;

    // Process the client request
    while (1) {
        // Clear the buffer
        memset(buf, 0, MAXDATASIZE);

        // Receive the file name from the client
        if ((recvbytes = recv(client_fd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv:");
            close(client_fd);
            pthread_exit(NULL);
        }

        // Check if the client requested to end the connection
        if (strcmp(buf, "fim") == 0) {
            printf("Client requested to end the connection.\n");
            close(client_fd);
            pthread_exit(NULL);
        }

        // Check if the file exists
        printf("Received file request: %s\n", buf);

        // Open the file to send the data to the client
        FILE *file = fopen(buf, "rb");
        // Check if the file exists
        if (file == NULL) {
            perror("fopen:");
            printf("File %s not found.\n", buf);
            // Send the file not found message to the client
            int file_not_found = 0;
            send(client_fd, &file_not_found, sizeof(file_not_found), 0);
            continue;
        }

        // Send the file found message to the client
        int file_found = 1;
        send(client_fd, &file_found, sizeof(file_found), 0);

        // Send the file data to the client
        while ((recvbytes = fread(buf, 1, MAXDATASIZE, file)) > 0) {
            // Send the file data to the client
            if ((sendbytes = send(client_fd, buf, recvbytes, 0)) == -1) {
                perror("send:");
                close(client_fd);
                fclose(file);
                pthread_exit(NULL);
            }
            // Print the number of bytes sent
            printf("Sent %d bytes of file data.\n", sendbytes);
        }

        // Close the file
        fclose(file);
        printf("File transfer completed.\n");

        // Send "done" marker to indicate the end of the file transfer as a
        // separate message
        strcpy(buf, "done");
        send(client_fd, buf, strlen(buf), 0);
    }

    close(client_fd);
    pthread_exit(NULL);
}
