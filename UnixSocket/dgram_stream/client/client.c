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
#define CLIPATH "/home/ducdn/code/UnixSocket/clidgramsock"

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
    memset(&client,0,sizeof(client));
    client.sun_family = AF_UNIX;
    strncpy(client.sun_path,CLIPATH,sizeof(client.sun_path) - 1);
    if (bind(sockfd,(SA*)&client, sizeof(client)) == -1)
    {
        perror("Bind");
    }
    else
    {
        printf("Bind successfully\n");
    }
    memset(&server,0,sizeof(server));
    server.sun_family = AF_UNIX;
    strncpy(server.sun_path,PATH,sizeof(server.sun_path) -1);
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
        if (recvfrom(sockfd,buffer,BUFFER_SIZE,0, NULL,NULL) == -1)
        {
            perror("Recvfrom");
            exit(1);
        }
        printf("From server: %s\n",buffer);
    }
    close(sockfd);
    return 0;
}