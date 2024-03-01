#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <netdb.h>

#define PORT 8080
#define SA struct sockaddr
#define BUFFER_SIZE 1024

void receive_list(int sockfd)
{
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    int nrecv;
    while (1)
    {
        nrecv = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (buffer[0] == '\0')
        {
            break;
        }
        printf("%s", buffer);
        bzero(buffer, BUFFER_SIZE);
    }
    return;
}

void receive_file(char file_name[], int sockfd)
{
    char buffer[BUFFER_SIZE] = {0};
    FILE *fp;
    int receive_flag = 1;
    fp = fopen(file_name, "wb");
    if (fp == NULL)
    {
        printf("Error in opening file");
        return;
    }
    int nrecv;
    while (receive_flag)
    {
        bzero(buffer, BUFFER_SIZE);
        nrecv = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (buffer[0] == '\0')
        {
            receive_flag = 0;
            break;
        }
        fwrite(buffer, sizeof(char), nrecv, fp);
    }
    fclose(fp);
    return;
}

int main(void)
{
    struct sockaddr_in servaddr, cli;
    int sockfd, connfd, len;
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    int initflag = 0;
    while (1)
    {
        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            printf("socket creation failed...\n");
            exit(0);
        }
        else if (initflag == 0)
        {
            initflag = 1;
            printf("Socket successfully created..\n");
        }
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);
        if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
        {
            printf("connection with the server failed...\n");
            exit(0);
        }
        else if (initflag == 0)
        {
            printf("connected to the server..\n");
        }
        bzero(buffer, BUFFER_SIZE);
        printf("Enter the string : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sockfd, buffer, sizeof(buffer), 0);
        if (strncmp(buffer, "exit", 4) == 0)
        {
            send(sockfd, buffer, sizeof(buffer), 0);
            break;
        }
        else if (strncmp(buffer, "list", 4) == 0)
        {
            receive_list(sockfd);
            printf("Received list successfully\n");
        }
        else if (strncmp(buffer, ".", 1) == 0)
        {
            strcpy(file_name, buffer);
            if (file_name[strlen(file_name) - 1] == '\n')
            {
                file_name[strlen(file_name) - 1] = '\0';
            }
            bzero(buffer, BUFFER_SIZE);
            receive_file(file_name, sockfd);
            printf("File received successfully\n");
        }
        else
        {
            bzero(buffer, BUFFER_SIZE);
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("From Server: %s\n", buffer);
        }
        close(sockfd);
    }
    close(sockfd);
    return 0;
}