#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define SA struct sockaddr
#define BUFFER_SIZE 1024
#define PATH "/home/ducdn/code/UnixSocket/dgramsock"

int main(void)
{
    struct sockaddr_un server;
    int sockfd;
    char buffer[BUFFER_SIZE];
    sockfd = socket(AF_UNIX,SOCK_DGRAM,0);
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
    strncpy(server.sun_path,PATH,sizeof(server.sun_path) - 1);
    int len = sizeof(struct sockaddr_un);
    while (1)
    {
        bzero(buffer,BUFFER_SIZE);
        printf("To server: ");
        fgets(buffer,BUFFER_SIZE,stdin);
        sendto(sockfd,buffer,BUFFER_SIZE,0,(SA*)&server,len);
        if (strncmp(buffer,"exit",4) == 0)
        {
            break;
        }
        bzero(buffer,BUFFER_SIZE);
        if (recvfrom(sockfd,buffer,BUFFER_SIZE,0, (SA*)&server, len) == -1)
        {
            perror("Recvfrom");
            exit(1);
        }
        printf("From server: %s\n",buffer);
    }
    close(sockfd);
    return 0;
}