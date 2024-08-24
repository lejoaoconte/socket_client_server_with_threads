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

// Define the server port
#define SERVPORT 3333
// Define the maximum data size
#define BACKLOG 10
// Define the maximum data size
#define MAXDATASIZE (5 * 1024)

// Function to process the client's request
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
    // Set the socket option
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

    // Listen for connections
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen:");
        exit(1);
    }
    printf("Start listen..... \n");

    while (1) {
        int client_fd;
        sin_size = sizeof(struct sockaddr);
        struct sockaddr_in peeraddr;
        int len = sizeof(peeraddr);
        char server_ip[20];

        // Accept a connection
        if ((client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr,
                                &sin_size)) == -1) {
            perror("accept:");
            continue;
        }
        printf("accept socket..... Client address: %s Port: %d\n",
               inet_ntoa(client_sockaddr.sin_addr),
               ntohs(client_sockaddr.sin_port));
        // Get the server's IP address
        getsockname(client_fd, (struct sockaddr *)&peeraddr, &len);
        inet_ntop(AF_INET, &peeraddr.sin_addr, server_ip, sizeof(server_ip));
        printf("accept socket..... Server address: %s \n", server_ip);

        // Create a thread to process the client's request
        if (pthread_create(&thread_id, NULL, process_client_thread,
                           &client_fd) != 0) {
            perror("pthread_create:");
            close(client_fd);
            continue;
        }
        // Detach the thread
        pthread_detach(thread_id);
    }
    // Close the socket
    close(sockfd);
}

void *process_client_thread(void *arg) {
    int client_fd = *(int *)arg;
    char buf[MAXDATASIZE];
    int recvbytes, sendbytes;

    while (1) {
        // Clear the buffer before use
        memset(buf, 0, MAXDATASIZE);

        // Receive the file name from the client
        if ((recvbytes = recv(client_fd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv:");
            close(client_fd);
            pthread_exit(NULL);
        }

        // Check if the client wants to end the connection
        if (strcmp(buf, "fim") == 0) {
            printf("Client requested to end the connection.\n");
            close(client_fd);
            pthread_exit(NULL);
        }

        // Print the file name received from the client
        printf("Received file request: %s\n", buf);

        // Open the file
        FILE *file = fopen(buf, "r");
        // Check if the file exists
        if (file == NULL) {
            perror("fopen:");
            printf("File %s not found.\n", buf);
            int file_not_found = 0;
            send(client_fd, &file_not_found, sizeof(file_not_found), 0);
            continue;
        }

        // Send the file to the client
        int file_exists = 1;
        // Send a message to the client indicating that the file exists
        send(client_fd, &file_exists, sizeof(file_exists), 0);

        // Send the file data to the client
        while ((recvbytes = fread(buf, 1, MAXDATASIZE, file)) > 0) {
            // Send the file data
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
        // Print a message indicating that the file transfer is complete
        printf("File transfer completed.\n");
    }

    // Close the client socket
    close(client_fd);
    // Exit the thread
    pthread_exit(NULL);
}
