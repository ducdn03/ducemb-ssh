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
#include <sys/select.h>

#define PORT 8080
#define SA struct sockaddr
#define BUFFER_SIZE 1024
#define MAX_CLIENT 50

void send_list(int connfd)
{
    DIR *d;
    struct dirent *dir;
    d = opendir("."); // Open the folder
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

int main(void)
{
    struct sockaddr_in servaddr, cli;
    int sockfd, connfd, len;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    int cli_socket[MAX_CLIENT];
    int acitvity, sd, max_sd;
    fd_set readfds;
    int i;
    memset(cli_socket, 0, sizeof(cli_socket));
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
    // set socket option to reuse the port
    /*if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(0);
    }*/

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        perror("socket bind failed...\n");
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
        printf("Server listening..\n");
    }
    len = sizeof(cli);
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        max_sd = sockfd;
        for (i = 0; i < MAX_CLIENT; i++)
        {
            sd = cli_socket[i];
            if (sd > 0)
            {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }
        acitvity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (acitvity < 0)
        {
            printf("Select error\n");
        }
        if (FD_ISSET(sockfd, &readfds))
        {
            connfd = accept(sockfd, (SA *)&cli, &len);
            if (connfd < 0)
            {
                exit(1);
            }
            printf("Connected from %s:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
            for (i = 0; i < MAX_CLIENT; i++)
            {
                if (cli_socket[i] == 0)
                {
                    cli_socket[i] = connfd;
                    break;
                }
            }
        }
        for (i = 0; i < MAX_CLIENT; i++)
        {
            if (FD_ISSET(cli_socket[i], &readfds))
            {
                connfd = cli_socket[i];
                bzero(buffer, BUFFER_SIZE);
                if (recv(connfd, buffer, BUFFER_SIZE, 0) > 0)
                {
                    printf("From client: %s\n", buffer);
                    if (strncmp(buffer, "exit", 4) == 0)
                    {
                        printf("Disconnected from %s:%d\n", inet_ntoa(cli.sin_addr),
                               ntohs(cli.sin_port));
                        close(connfd);
                        cli_socket[i] = 0;
                    }
                    else if (strncmp(buffer, "list", 4) == 0)
                    {
                        send_list(connfd);
                        bzero(buffer, BUFFER_SIZE);
                        buffer[0] = '\0';
                        send(connfd, buffer, BUFFER_SIZE, 0);
                        printf("Send list successfully...\n");
                    }
                    else if (strncmp(buffer, ".", 1) == 0)
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
                    else
                    {
                        bzero(buffer, BUFFER_SIZE);
                        printf("To client: ");
                        fgets(buffer, BUFFER_SIZE, stdin);
                        send(connfd, buffer, sizeof(buffer), 0);
                    }
                }
            }
        }
    }
    close(sockfd);
    close(connfd);
    return 0;
}
