#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <arpa/inet.h>

#define PORT 8080
#define SA struct sockaddr
#define BUFFER_SIZE     1024

void
send_file (FILE * fp,int connfd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer,'\0',BUFFER_SIZE);
    /*while (1)
    {
        int nread = fread(buffer,1,BUFFER_SIZE,fp);
        if (nread > 0)
        {
            printf("Sending \n");
            write(connfd,buffer,nread);
        }
        if (nread < 1024)
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
    }*/
    while (fgets(buffer,BUFFER_SIZE,fp) != NULL)
    {
        if (send(connfd,buffer,sizeof(buffer),0) == -1)
        {
            printf("Error in sending file...\n");
            break;
        }
        bzero(buffer,BUFFER_SIZE);
    }
    return;
}

int
main (void)
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    FILE * fp;
    char * file_name = "test.jpg";

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        return 0;
    }
    printf("Socket successfully created...\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // binding newly created socket to given IP and verification
    if (bind(sockfd, (SA*)&servaddr,sizeof(servaddr)) != 0)
    {
        printf("socket bind failed...\n");
        return 0;
    }
    printf("socket successfully binded...\n");

    //server is ready to listen and verification
    if (listen(sockfd,5) != 0)
    {
        printf("listen failed...\n");
        return 0;
    }
    printf("server listening...\n");
    len = sizeof(cli);

    //accept data packet from client and verification
    connfd = accept(sockfd,(SA*)&cli,&len);
    if (connfd < 0)
    {
        printf("server accept failed...\n");
        return 0;
    }
    printf("server accept the client...\n");

    //fp = fopen(file_name,"rb");
    fp = fopen(file_name,"r");
    if (fp == NULL)
    {
        printf("error in reading file...\n");
        return 0;
    }
    send_file(fp,connfd);
    printf("File data sent successfully...\n");
    close(sockfd);
    return 0;
}
