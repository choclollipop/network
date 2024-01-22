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
#define ADDR    "172.30.149.120"

#define BUFFER_SIZE 128

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
    struct sockaddr_in recvAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDR, &serverAddr.sin_addr.s_addr);

    socklen_t serverAddrLen = sizeof(serverAddr);
    socklen_t recvAddrLen = sizeof(recvAddrLen);


    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    char recvBuffer[BUFFER_SIZE];
    memset(recvBuffer, 0, sizeof(recvBuffer));

    strncpy(buffer, "abcdefACD", sizeof(buffer) - 1);

    sendto(socketFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddr, serverAddrLen);

    recvfrom(socketFd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr *)&recvAddr, &recvAddrLen);

    printf("recv:%s\n", recvBuffer);

    close(socketFd);
    
    

    return 0;
}