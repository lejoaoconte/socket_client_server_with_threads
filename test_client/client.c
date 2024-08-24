#include <error.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// Define the server port
#define SEVPORT 3333
// Define the maximum data size
#define MAXDATASIZE (1024 * 5)

int main(int argc, char *argv[]) {
    int sockfd, sendbytes, recvbytes;
    char buf[MAXDATASIZE];
    struct hostent *host;
    struct sockaddr_in serv_addr;

    // Check if the user entered the server's hostname
    if (argc < 2) {
        fprintf(stderr, "Please enter the server's hostname!\n");
        exit(1);
    }

    // Get the server's hostname
    if ((host = gethostbyname(argv[1])) == NULL) {
        perror("gethostbyname:");
        exit(1);
    }

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket:");
        exit(1);
    }

    // Set the server's address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SEVPORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);
    bzero(&(serv_addr.sin_zero), 8);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr,
                sizeof(struct sockaddr)) == -1) {
        perror("connect:");
        exit(1);
    }

    while (1) {
        // Get the file name from the user
        printf("Type file name: ");
        scanf("%s", buf);

        // Check if the user wants to end the connection
        if (strcmp(buf, "fim") == 0) break;

        // Send the file name to the server
        if ((sendbytes = send(sockfd, buf, strlen(buf), 0)) == -1) {
            perror("send:");
            exit(1);
        }

        int file_exists;
        // Check if the file exists on the server
        if ((recvbytes = recv(sockfd, &file_exists, sizeof(file_exists), 0)) ==
            -1) {
            perror("recv:");
            close(sockfd);
            exit(1);
        }

        // If the file does not exist, print an error message
        if (!file_exists) {
            printf("File not found on server.\n");
            continue;
        }

        // Receive the file from the server
        FILE *received_file = fopen(buf, "w");
        if (received_file == NULL) {
            perror("fopen:");
            exit(1);
        }

        // Write the received data to the file
        while ((recvbytes = recv(sockfd, buf, MAXDATASIZE, 0)) > 0) {
            fwrite(buf, sizeof(char), recvbytes, received_file);
            if (recvbytes < MAXDATASIZE) break;
        }

        // Check if there was an error while receiving the file
        if (recvbytes < 0) perror("recv:");

        // Close the file
        fclose(received_file);

        // Print a message if the file was received successfully
        if (recvbytes == 0) printf("File received successfully.\n");
    }

    close(sockfd);
}
