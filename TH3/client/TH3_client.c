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
#define BUFFER_SIZE     1024

void
send_file(char file_name[],int connfd)
{
    FILE * fp;
    char buffer[BUFFER_SIZE];
    fp = fopen(file_name,"r");
    if (fp == NULL)
    {
        return;
    }
    else
    {
        while (fgets(buffer,BUFFER_SIZE,fp) != NULL)
        {
            if ((-1) == send(connfd,buffer,sizeof(buffer),0))
            {
                printf("Error...");
            }
            bzero(buffer,BUFFER_SIZE);
        }
    }
}

void
receive_file(char file_name[], int sockfd)
{
    char buffer[BUFFER_SIZE] = {0};
    FILE * fp;
    fp = fopen(file_name, "wb");
    if (fp == NULL)
    {
    	printf("Error in opening file");
    	return;
    }
    int nrecv;
    while ((nrecv = recv(sockfd,buffer,BUFFER_SIZE,0)) > 0)
    {
    	fwrite(buffer,sizeof(char),nrecv,fp);
    }
    /* while (1)
    {
    	nrecv = recv(sockfd, buffer, BUFFER_SIZE, 0);
    	if (nrecv <= 0)
	    {
      	    break;
      	    return;
    	}
    	fprintf(fp, "%s", buffer);
    	bzero(buffer, BUFFER_SIZE);
    }*/
    return;
}

void func(int sockfd)
{
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    int n;
    int loop_flag = 1;
    while (loop_flag)
    {
        bzero(buffer, BUFFER_SIZE);
        printf("Enter the string : ");
        memset(file_name,'\0', sizeof(file_name));
        n = 0;
        while((buffer[n++] = getchar()) != '\n');
        send(sockfd,buffer,sizeof(buffer),0);
        if (strncmp(buffer,"exit",4) == 0)
        {
            loop_flag = 0;
            break;
        }
        strcpy(file_name,buffer);
        bzero(buffer,BUFFER_SIZE);
        receive_file(file_name, sockfd);
        printf("File received successfully\n");
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
    {
        printf("Socket successfully created..\n");
    }
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
    {
        printf("connected to the server..\n");
    }
    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
    return 0;
}