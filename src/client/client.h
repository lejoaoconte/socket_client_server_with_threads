#ifndef CLIENT_H
#define CLIENT_H

#define SEVPORT 3333            // Server port
#define MAXDATASIZE (1024 * 5)  // Max size of data buffer

void send_file_name(int sockfd, char* buf);
void receive_file(int sockfd, char* buf);

#endif
