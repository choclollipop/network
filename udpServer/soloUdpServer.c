#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
/* socketadd_in头文件 */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define PORT    6666

#define BUFFER_SIZE 128
#define DIFF        32

int main()
{
    /* 套接字 */
    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0)
    {
        perror("socket error");
        exit(-1);
    }

    struct sockaddr_in serverAddr;
    struct sockaddr_in clinetAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t serverAddrLen = sizeof(serverAddr);
    socklen_t clinetAddrLen = sizeof(clinetAddr);

    int ret = bind(socketFd, (struct sockaddr *)&serverAddr, serverAddrLen);
    if (ret < 0)
    {
        perror("socket error");
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int bytes = recvfrom(socketFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clinetAddr, &clinetAddrLen);
    
    for (int idx = 0; idx < bytes; idx++)
    {
        if (buffer[idx] >= 'a' && buffer[idx] <= 'z')
        {
            buffer[idx] = buffer[idx] - DIFF;
        }
    }

    sendto(socketFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clinetAddr, clinetAddrLen);

    close(socketFd);


    return 0;
}