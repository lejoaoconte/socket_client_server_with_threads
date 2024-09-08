#ifndef FILE_UTILS_H
#define FILE_UTILS_H

void send_file_name(int sockfd, char* buf);
void receive_file(int sockfd, char* buf);

#endif