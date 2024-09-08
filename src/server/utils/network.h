#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>

int create_socket();
void bind_socket(int sockfd, struct sockaddr_in *server_sockaddr);
void listen_for_connections(int sockfd);
int accept_client(int sockfd, struct sockaddr_in *client_sockaddr);

#endif
