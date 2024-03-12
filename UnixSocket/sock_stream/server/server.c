#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <errno.h>

#define PORT 8080
#define SA struct sockaddr
#define BUFFER_SIZE 1024
#define PATH 	"/home/ducdn/code/UnixSocket/mysock"

int main(void)
{
	struct sockaddr_un server,client;
	int sockfd, connfd, len;
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	char buffer[BUFFER_SIZE];
	if (sockfd == -1)
	{
		perror("Socket ");
		exit(1);
	}
	else
	{
		printf("Socket create successfully\n");
	}
	memset(&server,0,sizeof(server));
	if (remove(PATH) == -1 && errno != ENOENT)
	{
		perror("Remove\n");
		exit(1);
	}
	server.sun_family = AF_UNIX;
	strncpy(server.sun_path,PATH,sizeof(server.sun_path) -1);
	if (bind(sockfd, (SA*) &server,sizeof (server)) == -1)
	{
		perror("Bind ");
		exit(1);
	}
	else
	{
		printf("Bind successfully\n");
	}
	if (listen(sockfd,5) == -1)
	{
		perror("Listen ");
		exit(1);
	}
	else
	{
		printf("Server is listenning\n");
	}
	connfd = accept(sockfd,NULL, NULL);
	while (1)
	{
		bzero(buffer,BUFFER_SIZE);
		if (recv(connfd,buffer, BUFFER_SIZE,0) < 0)
		{
			break;
		}
		if (strncmp(buffer,"exit",4) == 0)
		{
			printf("server exit\n");
			break;
		}
		printf("From client: %s\n",buffer);
		printf("To client: ");
		fgets(buffer,BUFFER_SIZE,stdin);
		send(connfd,buffer,BUFFER_SIZE,0);
	}
	close(sockfd);
	close(connfd);
	return 0;
}
