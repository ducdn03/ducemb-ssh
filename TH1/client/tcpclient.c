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
#define BUFFER_SIZE 80
#define BUFF_SEND_SIZE 1024

void
send_file(char file_name[],int connfd)
{
    FILE * fp;
    char buffer[BUFF_SEND_SIZE];
    fp = fopen(file_name,"r");
    if (fp == NULL)
    {
        return;
    }
    else
    {
        while (fgets(buffer,BUFF_SEND_SIZE,fp) != NULL)
        {
            if ((-1) == send(connfd,buffer,sizeof(buffer),0))
            {
                printf("Error...");
            }
            bzero(buffer,BUFF_SEND_SIZE);
        }
    }
}

void receive_file(char file_name[], int sockfd)
{
    FILE *fp;
    char buffer[BUFF_SEND_SIZE];
    fp = fopen(file_name, "w+");
    int loop_flag = 1;
    int key;
    if (fp == NULL)
    {
        return;
    }
    while (loop_flag)
    {
        key = recv(sockfd, buffer, BUFF_SEND_SIZE, 0);
        if (key <= 0)
        {
            loop_flag = 0;
            break;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, BUFF_SEND_SIZE);
    }
    return;
}

void func(int sockfd)
{
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    memset(file_name, '\0', sizeof(file_name));
    int n;
    int loop_flag = 1;
    int mode = 0;
    while (loop_flag)
    {
        bzero(buffer, BUFFER_SIZE);
        printf("Enter the string : ");
        n = 0;
        while ((buffer[n++] = getchar()) != '\n');
        if (strcmp(buffer, "get") == 0)
        {
            mode = 1;
        }
        else if (strcmp(buffer, "put") == 0)
        {
            mode = 2;
        }
        else if (strcmp(buffer, "exit") == 0)
        {
            mode = 3;
        }
        else
        {
            mode = 0;
        }
        swtich(mode)
        {
        case 1:
            bzero(buffer, BUFFER_SIZE);
            printf("Enter the file name : ");
            n = 0;
            while ((buffer[n++] = getchar()) != '\n')
                ;
            strcpy(file_name, buffer);
            write(sockfd, buffer, sizeof(buffer));
            receive_file(file_name, sockfd);
            break;
        case 2:
            bzero(buffer, BUFFER_SIZE);
            printf("Enter the file name : ");
            n = 0;
            while ((buffer[n++] = getchar()) != '\n');
            strcpy(file_name, buffer);
            write(sockfd, buffer, sizeof(buffer));
            send_file(file_name, sockfd);
            break;
        case 3:
            loop_flag = 0;
            break;
        default:
            write(sockfd, buffer, sizeof(buffer));
            bzero(buffer, BUFFER_SIZE);
            read(sockfd, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            break;
        }
    }
    return;
}

int main(void)
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
    return 0;
}