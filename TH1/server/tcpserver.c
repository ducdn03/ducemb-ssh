#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#define PORT 8080 
#define SA struct sockaddr
#define BUFFER_SIZE     80
#define BUFF_SEND_SIZE  1024

void
write_buffer (char buffer[],const char source[])
{
    int i = 0;
    for (i = 0;i < BUFFER_SIZE;i++)
    {
        buffer[i] = '\0';
    }
    for (i = 0;i < strlen(source);i++)
    {
        buffer[i] = source[i];
    }
}

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

void
func (int connfd)
{
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    memset(file_name,'\0',sizeof(file_name));
    int n;
    int loop_flag = 1;
    int mode = 0;
    while (loop_flag)
    {
        bzero(buffer,BUFFER_SIZE);
        read(connfd,buffer,sizeof(buffer));
        if (strcmp(buffer,"get") == 0)
        {
            bzero(buffer,BUFFER_SIZE);
            write_buffer(buffer,"Enter file name: ");
            write(connfd,buffer,sizeof(buffer));
            bzero(buffer,BUFFER_SIZE);
            read(connfd,buffer,sizeof(buffer));
            strcpy(file_name,buffer);
            send_file(file_name,connfd);
        }
        else if (strcmp(buffer,"exit") == 0)
        {
            loop_flag = 0;
            break;
        }
    }
}

int main(void)
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli; 
   
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
   
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT); 
   
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
   
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n");
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
   
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server accept failed...\n"); 
        exit(0);
    } 
    else
        printf("server accept the client...\n"); 
   
    // Function for chatting between client and server 
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
    return 0;
}