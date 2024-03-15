#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>

#define SA struct sockaddr
#define BUFFER_SIZE 1024

int main(void)
{
    int sockfd, len;
    struct sockaddr_un server;
    char buffer[BUFFER_SIZE];
    sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("Socket");
        exit(1);
    }
    else
    {
        printf("Socket create successfully\n");
    }
    memset(&server,0,sizeof(server));
    server.sun_family = AF_UNIX;
    server.sun_path[0] = '\0';
    strncpy(&server.sun_path[1],"abstract",sizeof(server.sun_path) - 2);
    if (connect(sockfd,(SA*)&server,sizeof(struct sockaddr_un)) == -1)
    {
        perror("Connect");
        exit(1);
    }
    else
    {
        printf("Connected successfully\n");
    }
    while (1)
    {
        bzero(buffer,BUFFER_SIZE);
        printf("To server: ");
        fgets(buffer,BUFFER_SIZE,stdin);
        send(sockfd,buffer,BUFFER_SIZE,0);
        if (strncmp(buffer, "exit" ,4) == 0)
        {
            break;
        }
        bzero(buffer,BUFFER_SIZE);
        recv(sockfd,buffer, BUFFER_SIZE,0);
        printf("From server: %s",buffer);
    }
    close(sockfd);
    return 0;
}