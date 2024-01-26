#include <dirent.h>

void
receive_list(int sockfd)
{
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    int nrecv = recv(sockfd,buffer,BUFFER_SIZE,0);
    while (n > 0)
    {
        printf("%s\n", buffer);
        bzero(buffer, BUFFER_SIZE);
        nrecv = recv(sockfd,buffer,BUFFER_SIZE,0);
    }
}

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
            strcpy(buffer,dir->d_name);
            send(connfd,buffer,len,0);
        }
        closedir(d);
    }
    return;
}