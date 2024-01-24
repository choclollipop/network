#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <ctype.h>

#define SERVER_PORT    8888
#define LISTEN_MAX     128
#define BUFFER_SIZE    128

/* 用单进程/线程实现并发 */
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }

    struct sockaddr_in localAddress;
    struct sockaddr_in clientAddress;
    bzero((void *)&localAddress, sizeof(localAddress));
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(SERVER_PORT);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t localAddressLen = sizeof(localAddress);
    socklen_t clientAddressLen = sizeof(clientAddress);


    int ret = bind(sockfd, (struct sockaddr *)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        close(sockfd);
        exit(-1);
    }

    ret = listen(sockfd, LISTEN_MAX);
    if (ret == -1)
    {
        perror("listen error");
        close(sockfd);
        exit(-1);
    }

    
    fd_set readSet, copySet;
    /* 清空集合 */
    FD_ZERO(&readSet);
    bzero(&copySet, sizeof(copySet));
    
    /* 把监听的文件描述符添加到读集合中，让内核帮忙检测 */
    FD_SET(sockfd, &readSet);

    int maxfd = sockfd;
    char buffer[BUFFER_SIZE];
    bzero(&buffer, sizeof(buffer));

#if 0
    /* 设置超时 */
    struct timeval timeValue;
    bzero(&timeValue, sizeof(timeValue));
    timeValue.tv_sec = 5;
    timeValue.tv_usec = 0;

#endif

    while (1)
    {
        /* 备份读集合 */
        copySet = readSet;
        ret = select(maxfd + 1, &copySet, NULL, NULL, NULL);
        if (ret == -1)
        {
            perror("select error");
            break;
        }
#if 0
        else if (ret == 0)
        {
            /* 超时 */
        }
#endif
        if (FD_ISSET(sockfd, &copySet))
        {
            int acceptFd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen);
            if (acceptFd == -1)
            {
                perror("accept error");
                break;
            }

            /* 将通信的句柄放在读集合中 */
            FD_SET(acceptFd, &readSet);

            /* 更新maxfd */
            maxfd = maxfd < acceptFd ? acceptFd : maxfd;
        }

        for (int idx = 3; idx <= maxfd; idx++)
        {
            if (idx != sockfd && FD_ISSET(idx, &copySet))
            {
                /* 此时一定有客户端要求通信 */
                ssize_t readBytes = read(idx, &buffer, sizeof(buffer));
                if (readBytes < 0)
                {
                    perror("read error");
                    /* 将该通信句柄从读集合中删掉 */
                    FD_CLR(idx, &readSet);
                    close(idx);
                    continue;
                }

                else if (readBytes == 0)
                {
                    printf("客户端断开连接....\n");
                    FD_CLR(idx, &readSet);
                    close(idx);
                    continue;
                }

                else
                {
                    printf("recv:%s\n", buffer);

                    for (int jdx = 0; jdx < readBytes; jdx++)
                    {
                        buffer[jdx] = toupper(buffer[jdx]);
                    }

                    write(idx, buffer, readBytes);
                }
            }
        }
    }

    /* 关闭文件描述符 */
    close(sockfd);

    return 0;
}