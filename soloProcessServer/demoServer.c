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

#define MAX_LISTEN  128
#define LOCAL_IPADDRESS "172.30.149.120"
#define SERVER_PORT 5555
#define BUFFER_SIZE 128

// void signalHandler(int sigNum)
// {
//     int ret = 0;
//     /* 回收资源 todo.... */

// }

int main()
{
    // /* 捕捉信号 */
    // signal(SIGINT, signalHandler);
    // signal(SIGQUIT, signalHandler);
    // signal(SIGTSTP, signalHandler);

    /* 创建套接字 */
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        perror("socket error");
        exit(-1);
    }

    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));

    localAddr.sin_family = AF_INET;
    /* 转成大端 */
    localAddr.sin_port = htons(SERVER_PORT);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* 不用默认地址 */
#if 0
    inet_pton(AF_INET, LOCAL_IPADDRESS, &localAddr.sin_addr.s_addr);
#endif 

    int localAddrLen = sizeof(localAddr);

    /* 绑定 */
    int ret = bind(socketfd, (struct sockaddr *) &localAddr, localAddrLen);
    if (ret == -1)
    {
        perror("bind error");
        close(socketfd);
        exit(-1);
    }

    /* 监听 */
    ret = listen(socketfd, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        close(socketfd);
        exit(-1);
    }

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    
    socklen_t clientAddrLen = 0;
    
    int acceptfd = accept(socketfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (acceptfd == -1)
    {
        perror("listen error");
        close(socketfd);
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int readBytes = 0;

    char repalybuffer[BUFFER_SIZE];
    memset(repalybuffer, 0, sizeof(repalybuffer));

    while (1)
    {
        readBytes = read(acceptfd, buffer, sizeof(buffer) - 1);
        if (readBytes <= 0)
        {
            /* 出错或通信结束都关闭此通信句柄 */
            perror("read error");
            close(acceptfd);
            break;
        }
        else
        {
            /* 读到的字符串 */
            printf("buffer : %s\n", buffer);
            sleep(3);

            strncpy(repalybuffer, "一起加油", sizeof(repalybuffer) - 1);
            write(acceptfd, repalybuffer, strlen(repalybuffer));
        }
    }

    close(socketfd);

    return 0;
}