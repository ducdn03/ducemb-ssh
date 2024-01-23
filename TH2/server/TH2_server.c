#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT    10080
#define SA struct sockaddr
#define BUFFER_SIZE 1024

int main(void)
{
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    int nrecv;
    FILE * fp;
    char buffer[BUFFER_SIZE];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // create socket using UDP
    if (sockfd < 0)
    {
        perror("socket creation failed...\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully created..\n");
    }

    // assign IP, PORT
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // bind socket to IP, PORT
    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("socket bind failed...\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully binded..\n");
    }
    len = sizeof(cliaddr);
    while (1)
    {
        printf("Waiting for file name...\n");
        bzero(buffer, BUFFER_SIZE);
        nrecv = recvfrom(sockfd,buffer,BUFFER_SIZE,0,(SA*)&cliaddr,&len);
        if (nrecv <= 0 || strncmp(buffer, "exit", 4) == 0)
        {
            break;
        }
        else
        {
            strcpy(buffer, strtok(buffer, "\n"));
            fp = fopen(buffer, "r");
            bzero(buffer, BUFFER_SIZE);
            while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
            {
                if (sendto(sockfd, buffer, BUFFER_SIZE, 0,\
                (SA*)&cliaddr, len) == -1)
                {
                    perror("Error in sending file...\n");
                    break;
                }
                bzero(buffer, BUFFER_SIZE);
            }
            buffer[0] = EOF;
            sendto(sockfd, buffer, BUFFER_SIZE, 0,\
            (SA*)&cliaddr, len);
            printf("Send file finished...\n");
        }
    }
    close(sockfd);
    return 0;
}