#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../server.h"

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
        if (file == NULL) {
            perror("fopen:");
            printf("File %s not found.\n", buf);
            int file_not_found = 0;
            send(client_fd, &file_not_found, sizeof(file_not_found), 0);
            continue;
        }

        // File exists, send acknowledgment
        int file_exists = 1;
        send(client_fd, &file_exists, sizeof(file_exists), 0);

        // Send the file data
        while ((recvbytes = fread(buf, 1, MAXDATASIZE, file)) > 0) {
            if ((sendbytes = send(client_fd, buf, recvbytes, 0)) == -1) {
                perror("send:");
                close(client_fd);
                fclose(file);
                pthread_exit(NULL);
            }
            printf("Sent %d bytes of file data.\n", sendbytes);
        }

        fclose(file);
        printf("File transfer completed.\n");
    }

    // Close the client socket
    close(client_fd);
    pthread_exit(NULL);
}
