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
    struct sockaddr_un server,client;
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
    if (remove(PATH) == -1 && errno != ENOENT)
    {
        perror("Remove");
        exit(1);
    }
    memset(&server,0,sizeof(server));
    server.sun_family = AF_UNIX;
    strncpy(server.sun_path,PATH,sizeof(server.sun_path) - 1);
    if (bind(sockfd,(SA*) &server,sizeof(server)) == -1)
    {
        perror("Bind");
        exit(1);
    }
    else
    {
        printf("Bind successfully\n");
    }
    while (1)
    {
        int len = sizeof(struct sockaddr_un);
        bzero(buffer,BUFFER_SIZE);
        if (recvfrom(sockfd,buffer,BUFFER_SIZE,0,(SA*)&client, &len) == -1)
        {
            perror("Recvfrom");
            exit(1);
        }
        else
        {
            printf("From client: %s\n",buffer);
        }
        if (strncmp(buffer,"exit",4) == 0)
        {
            printf("Server exit\n");
            break;
        }
        bzero(buffer,BUFFER_SIZE);
        printf("To client: ");
        fgets(buffer,BUFFER_SIZE,stdin);
        if (sendto(sockfd,buffer,BUFFER_SIZE,0,(SA*)&client,len) == -1)
        {
            perror("Sendto");
            exit(1);
        }
    }
    close(sockfd);
    return 0;
}