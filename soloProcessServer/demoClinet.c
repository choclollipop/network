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

#define SERVER_PORT 5555
#define SERVER_IP "172.30.149.120"
#define BUFFER_SIZE 128

int main()
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        perror("socket error");
        _exit(-1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    
    int ret = inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);
    if (ret != 1)
    {
        perror("inet_pton error");
        close(socketfd);
        _exit(-1);
    }

    int serverAddrLen = sizeof(serverAddr);

    ret = connect(socketfd, (struct sockaddr *)&serverAddr, serverAddrLen);
    if (ret == -1)
    {
        perror("connect error");
        close(socketfd);
        _exit(-1);
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    char recvBuffer[BUFFER_SIZE];
    memset(recvBuffer, 0, sizeof(recvBuffer));

    while (1)
    {
        strncpy(buffer, "加油 254", sizeof(buffer) - 1);

        write(socketfd, buffer, sizeof(buffer));

        read(socketfd, recvBuffer, sizeof(recvBuffer) - 1);
        printf("recv : %s\n", recvBuffer);
    }
    

    sleep(5);

    close(socketfd);

    return 0;
}