#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SA struct sockaddr
#define BUFFER_SIZE 1024
#define SOCK_PATH "/home/ducdn/code/UnixSocket/mysock"
int main(void)
{
    int sockfd, connfd, len;
    struct sockaddr_un serveraddr, cli;
    char buffer[BUFFER_SIZE];
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket creation failed...\n");
        exit(1);
    }
    else
    {
        printf("Socket successfully created..\n");
    }
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strncpy(serveraddr.sun_path, SOCK_PATH, sizeof(serveraddr.sun_path) - 1);
    if (connect(sockfd,(SA*) &serveraddr,sizeof(struct sockaddr_un)) == -1)
    {
        perror("Connect\n");
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