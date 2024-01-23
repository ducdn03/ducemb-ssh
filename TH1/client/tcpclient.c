#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

#define PORT 8080
#define SA struct sockaddr
#define BUFFER_SIZE   1024

void receive_file(int sockfd)
{
    char buffer[BUFFER_SIZE] = {0};
    FILE * fp;
    //fp = fopen("receive.jpg", "wb");
    fp = fopen("receive.txt", "w");
    if (fp == NULL)
    {
        printf("Error in opening file");
        return;
    }
    int nrecv;
    /*while ((nrecv = recv(sockfd,buffer,BUFFER_SIZE,0)) > 0)
    {
        fwrite(buffer,sizeof(char),nrecv,fp);
    }*/
    while (1)
    {
        nrecv = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (nrecv <= 0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, BUFFER_SIZE);
    }
    return;
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed...\n");
        exit(1);
    }
    else
    {
        printf("Socket successfully created...\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Connection with the server failed...\n");
        exit(1);
    }
    else
    {
        printf("Connected to the server...\n");
    }

    receive_file(sockfd);
    printf("File received successfully...\n");

    close(sockfd);
    return 0;
}
