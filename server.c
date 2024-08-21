#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>

#define SERVPORT 3333
#define BACKLOG 10
#define MAX_CONNECTED_NO 10
#define MAXDATASIZE (5*1024)

void *process_client_thread(void *);

int main()
{
	struct sockaddr_in server_sockaddr,client_sockaddr;
	int sin_size;
	int sockfd;//,*client_fd;
	pthread_t thread_id;

	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) == -1)
	{
		perror("socket:");
		exit(1);
	}

	int on;
	on = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	server_sockaddr.sin_family=AF_INET;
	server_sockaddr.sin_port = htons(SERVPORT);
	server_sockaddr.sin_addr.s_addr=INADDR_ANY;
	memset(&(server_sockaddr.sin_zero),0,8);

	if((bind(sockfd,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr))) == -1)	
	{
		perror("bind:");
		exit(1);
	}

	if(listen(sockfd,BACKLOG) == -1)
	{
		perror("listen:");
		exit(1);
	}
	printf("Start listen..... \n");


	while(1){
		int client_fd;
		sin_size = sizeof(struct sockaddr);
		// client_fd = malloc(sizeof(int)); 
		struct sockaddr_in peeraddr;
		int len = sizeof(peeraddr);
		char server_ip[20];

		if((client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, &sin_size)) == -1) {
				perror("accept:");
				// free(client_fd); 
				continue;
		}
		printf("accept socket..... Client address: %s Port: %d\n", inet_ntoa(client_sockaddr.sin_addr), ntohs(client_sockaddr.sin_port));
		getsockname(client_fd,(struct sockaddr *)&peeraddr,&len);
		inet_ntop(AF_INET, &peeraddr.sin_addr,server_ip,sizeof(server_ip));
		printf("accept socket..... Server address: %s \n",server_ip);	

		if(pthread_create(&thread_id, NULL, process_client_thread, &client_fd) != 0) {
				perror("pthread_create:");
				close(client_fd);
				// free(client_fd);
				continue;
		}
		pthread_detach(thread_id);
	}
	close(sockfd);
}

void *process_client_thread(void *arg) {
		int client_fd = *(int*)arg;
    while(1) {
			char buf[MAXDATASIZE];
			int recvbytes, sendbytes;

			if((recvbytes=recv(client_fd,buf,MAXDATASIZE,0)) == -1)
			{
				perror("recv:");	
				close(client_fd);
    		pthread_exit(NULL);
			}
			printf("Recv bytes: %d, buf: %s\n",recvbytes, buf);

			int tam = 0;
			FILE *file = fopen(buf, "r");
			if (file == NULL) {
				send(client_fd,&tam,sizeof(tam),0);
				continue;
			}

			if((sendbytes = send(client_fd,buf,sizeof(buf),0)) == -1)
			{
				perror("send:");
				close(client_fd);
				exit(1);
			}

			printf("Send bytes: %d\n", sendbytes);

		}
		close(client_fd);
    pthread_exit(NULL);
}
