#ifndef SERVER_H
#define SERVER_H

#define SERVPORT 3333           // Server port
#define BACKLOG 10              // Max number of pending connections
#define MAXDATASIZE (5 * 1024)  // Max size of data buffer

void *process_client_thread(void *arg);

#endif