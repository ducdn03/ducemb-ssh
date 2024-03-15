#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>

#define SA struct sockaddr
#define BUFFER_SIZE 1024

int main(void)
{
    int sockfd,connfd;
    struct sockaddr_un server,client;
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
    if (bind(sockfd,(SA*)&server,sizeof(server)) == -1)
    {
        perror("Bind");
        exit(1);
    }
    else
    {
        printf("Bind successfully\n");
    }
    if (listen(sockfd,5) == -1)
    {
        perror("Listen");
        exit(1);
    }
    else
    {
        printf("Server listening\n");
    }
    connfd = accept(sockfd,NULL,NULL);
    if (connfd < 0)
    {
        perror("Accept");
        exit(1);
    }
    else
    {
        printf("Server accept the client\n");
    }
    while (1)
    {
        bzero(buffer,BUFFER_SIZE);
        if (recv(connfd,buffer, BUFFER_SIZE,0) < 0)
        {
            perror("Recv");
            exit(1);
        }
        printf("From client: %s",buffer);
        if (strncmp(buffer,"exit",4) == 0)
        {
            printf("Server exit");
            break;
        }
        printf("To client: ");
        fgets(buffer,BUFFER_SIZE,stdin);
        if (send(connfd,buffer,BUFFER_SIZE,0) < 0)
        {
            perror("Send");
            exit(1);
        }
    }
    close(sockfd);
    close(connfd);
    return 0;
}