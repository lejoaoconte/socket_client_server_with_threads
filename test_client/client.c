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

#define SEVPORT 3333
#define MAXDATASIZE (1024 * 5)

int main(int argc, char *argv[]) {
    int sockfd, sendbytes, recvbytes;
    char buf[MAXDATASIZE];
    struct hostent *host;
    struct sockaddr_in serv_addr;

    // Create a socket and connect to the server (if the server exists)
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

    // Connect to the server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SEVPORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);
    bzero(&(serv_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&serv_addr,
                sizeof(struct sockaddr)) == -1) {
        perror("connect:");
        exit(1);
    }

    // Send the file name to the server and receive the file
    while (1) {
        // Get the file name from the user
        printf("Type file name: ");
        scanf("%s", buf);

        // If the user types "fim", the client will exit
        if (strcmp(buf, "fim") == 0) break;

        // Send the file name to the server
        if ((sendbytes = send(sockfd, buf, strlen(buf), 0)) == -1) {
            perror("send:");
            exit(1);
        }

        // Receive the file from the server
        int file_exists;
        if ((recvbytes = recv(sockfd, &file_exists, sizeof(file_exists), 0)) ==
            -1) {
            perror("recv:");
            close(sockfd);
            exit(1);
        }

        // Check if the file exists on the server
        if (!file_exists) {
            printf("File not found on server.\n");
            continue;
        }

        // Open the file to write the received data
        FILE *received_file = fopen(buf, "wb");
        if (received_file == NULL) {
            perror("fopen:");
            exit(1);
        }

        // Receive the file data from the server
        while (1) {
            if ((recvbytes = recv(sockfd, buf, MAXDATASIZE, 0)) > 0) {
                // Check if the "done" marker is received
                if (strcmp(buf, "done") == 0) {
                    break;
                }
                fwrite(buf, 1, recvbytes, received_file);
            }
        }

        // Close the file
        fclose(received_file);
        printf("File received successfully.\n");
    }

    close(sockfd);
}
