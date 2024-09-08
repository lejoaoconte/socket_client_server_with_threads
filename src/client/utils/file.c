#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../client.h"

void send_file_name(int sockfd, char* buf) {
    int sendbytes;

    // Send file name to server
    if ((sendbytes = send(sockfd, buf, strlen(buf), 0)) == -1) {
        perror("send:");
        exit(1);
    }
}

void receive_file(int sockfd, char* buf) {
    int recvbytes, file_exists;

    // Check if file exists on the server
    if ((recvbytes = recv(sockfd, &file_exists, sizeof(file_exists), 0)) ==
        -1) {
        perror("recv:");
        close(sockfd);
        exit(1);
    }

    if (!file_exists) {
        printf("File not found on server.\n");
        return;
    }

    // If file exists, receive it
    FILE* received_file = fopen(buf, "w");
    if (received_file == NULL) {
        perror("fopen:");
        exit(1);
    }

    // Save the file received from the server
    while ((recvbytes = recv(sockfd, buf, MAXDATASIZE, 0)) > 0) {
        fwrite(buf, sizeof(char), recvbytes, received_file);
        if (recvbytes < MAXDATASIZE) break;
    }

    if (recvbytes < 0) perror("recv:");

    fclose(received_file);

    if (recvbytes == 0) printf("File received successfully.\n");
}
