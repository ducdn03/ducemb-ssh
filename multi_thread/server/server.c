#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SA struct sockaddr
#define PORT 8080
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

void send_list(int connfd)
{
    DIR *d;
    struct dirent *dir;
    d = opendir("/home/ducdn/code/ducemb-ssh/multiprocess/server"); // Open the folder
    int len;
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    if (d != 0)
    {
        while ((dir = readdir(d)) != NULL)
        {
            len = strlen(dir->d_name);
            if (len > 2)
            {
                if (send(connfd, dir->d_name, len, 0) < 0)
                {
                    perror("Failed to write to socket");
                    return;
                }
                send(connfd, "\n", 1, 0);
            }
        }
        closedir(d);
    }
    return;
}

int send_file(char file_name[], int connfd)
{
    FILE *fp;
    fp = fopen(file_name, "rb");
    unsigned long long filesize = 0;
    if (fp == NULL)
    {
        perror("Failed to open file");
        return -1;
    }
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    while (1)
    {
        int nread = fread(buffer, 1, BUFFER_SIZE, fp);
        if (nread > 0)
        {
            // printf("Sending \n");
            filesize += nread;
            if (send(connfd, buffer, nread, 0) < 0)
            {
                perror("Failed to write to socket");
                return -1;
            }
            bzero(buffer, BUFFER_SIZE);
        }
        else
        {
            break;
        }
        if (feof(fp))
        {
            printf("End of file \n");
            break;
        }
    }
    printf("Send : %lld bytes\n", filesize);
    return 0;
}

void *handle_client(int connfd)
{
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    int nrecv;
    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        nrecv = recv(connfd, buffer, BUFFER_SIZE, 0);
        if (nrecv > 0)
        {
            printf("From client: %s\n", buffer);
            if (strncmp(buffer, "exit", 4) == 0)
            {
                close(connfd);
                break;
            }
            else if (strncmp(buffer, "list", 4) == 0)
            {
                send_list(connfd);
                bzero(buffer, BUFFER_SIZE);
                buffer[0] = '\0';
                send(connfd, buffer, BUFFER_SIZE, 0);
                printf("Send list successfully...\n");
            }
            else
            {
                strcpy(file_name, buffer);
                if (file_name[strlen(file_name) - 1] == '\n')
                {
                    file_name[strlen(file_name) - 1] = '\0';
                }
                printf("File Name : %s\n", file_name);
                bzero(buffer, BUFFER_SIZE);
                send_file(file_name, connfd);
                bzero(buffer, BUFFER_SIZE);
                buffer[0] = '\0';
                send(connfd, buffer, BUFFER_SIZE, 0);
                printf("Send successfully...\n");
            }
        }
    }
    close(connfd);
    return NULL;
}

int main(void)
{
    struct sockaddr_in servaddr, cli;
    int sockfd, connfd, len;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    pthread_t threadid;
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully binded..\n");
    }
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
    {
        printf("Server listening...\n");
    }
    len = sizeof(cli);
    while (1)
    {
        connfd = accept(sockfd, (SA *)&cli, &len);
        if (connfd < 0)
        {
            perror("Failed to accept connection");
            exit(1);
        }
        printf("Connected from %s:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
        pthread_create(&threadid, NULL, handle_client, connfd);
    }
    close(sockfd);
    close(connfd);
    return 0;
}
