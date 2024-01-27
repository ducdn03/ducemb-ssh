#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#include <dirent.h>
#define PORT 8080 
#define SA struct sockaddr
#define BUFFER_SIZE     1024

void
send_list(int connfd)
{
    DIR * d;
    struct dirent * dir;
    d = opendir("."); //Open the folder
    int len;
    char buffer[BUFFER_SIZE];
    memset(buffer,'\0',BUFFER_SIZE);
    if (d != 0)
    {
        while ((dir = readdir(d)) != NULL)
        {
            len = strlen(dir->d_name);
            if (len > 2)
            {
                if (send(connfd,dir->d_name,len,0) < 0)
                {
                    perror("Failed to write to socket");
                    return;
                }
            }
        }
        closedir(d);
    }
    return;
}

int
send_file(char file_name[],int connfd)
{
    FILE * fp;
    fp = fopen(file_name,"rb");
    if (fp == NULL)
    {
        perror("Failed to open file");
        return -1;
    }
    char buffer[BUFFER_SIZE];
    memset(buffer,'\0',BUFFER_SIZE);
    while (1)
    {
        int nread = fread(buffer,1,BUFFER_SIZE,fp);
        if (nread > 0)
        {
            printf("Sending \n");
            if (send(connfd,buffer,nread,0) < 0)
            {
                perror("Failed to write to socket");
                return -1;
            }
        }
        if (nread < BUFFER_SIZE)
        {
            if (feof(fp))
            {
                printf("End of file \n");
            }
            if (ferror(fp))
            {
                printf("error reading \n");
            }
            break;
        }
    }
    return 0;
}

void
func (int connfd)
{
    char buffer[BUFFER_SIZE];
    char file_name[BUFFER_SIZE];
    memset(file_name,'\0',sizeof(file_name));
    int n;
    int loop_flag = 1;
    while (loop_flag)
    {
        bzero(buffer,BUFFER_SIZE);
        recv(connfd,buffer,BUFFER_SIZE,0);
        if (strncmp(buffer,"exit",4) == 0)
        {
            loop_flag = 0;
            break;
        }
        else if (strncmp(buffer,"list",4) == 0)
        {
            send_list(connfd);
            printf("Send list successfully...\n");
        }
        else
        {
            strcpy(file_name,buffer);
            if (file_name[strlen(file_name) - 1] == '\n')
            {
                file_name[strlen(file_name) - 1] = '\0';
            }
            printf("File Name : %s\n",file_name);
            bzero(buffer,BUFFER_SIZE);
            send_file(file_name,connfd);
            printf("Send successfully...\n");
        }
    }
    return;
}

int main(void)
{
    int sockfd, connfd, len;
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT); 
   
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
    { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
    {
        printf("Socket successfully binded..\n"); 
    }
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n");
        exit(0); 
    } 
    else
    {
        printf("Server listening..\n");
    }
    len = sizeof(cli); 
   
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0)
    { 
        printf("server accept failed...\n"); 
        exit(0);
    } 
    else
    {
        printf("server accept the client...\n"); 
    }
    // Function for chatting between client and server 
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
    return 0;
}