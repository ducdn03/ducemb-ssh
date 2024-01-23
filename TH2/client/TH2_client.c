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
    struct sockaddr_in servaddr;
    FILE * fp;
    char buffer[BUFFER_SIZE];
    int nrecv;
    socklen_t len;
    int i;
    int receive_flag = 0;
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
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    len = sizeof(servaddr);
    while (1)
    {
        printf("Enter file name: ");
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        if (strncmp(buffer,"exit",4) == 0)
        {
            sendto(sockfd, buffer, BUFFER_SIZE, 0,\
            (SA*)&servaddr, len);
            printf("Client Exit...\n");
            break;
        }
        else
        {
            strcpy(buffer, strtok(buffer, "\n"));
            fp = fopen(buffer, "w");
            sendto(sockfd, buffer, BUFFER_SIZE, 0,\
            (SA*)&servaddr, len);
            bzero(buffer,BUFFER_SIZE);
            receive_flag = 1;
            while (receive_flag)
            {
                nrecv = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,\
                (SA*)&servaddr, &len);
                for (i = 0;i < BUFFER_SIZE;i++)
                {
                    if (buffer[i] == EOF)
                    {
                        receive_flag = 0;
                        break;
                    }
                }
                if (receive_flag)
                {
                    fprintf(fp, "%s", buffer);
                }
                bzero(buffer, BUFFER_SIZE);
            }
            printf("Receive file finished...\n");
            receive_flag = 0;
            fclose(fp);
        }
    }
    close(sockfd);
    return 0;
}