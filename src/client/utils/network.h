#ifndef NETWORK_H
#define NETWORK_H

#include <netdb.h>
#include <netinet/in.h>

int create_socket_and_connect(struct hostent* host,
                              struct sockaddr_in* serv_addr);

#endif