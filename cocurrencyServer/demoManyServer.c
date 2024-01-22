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
#include <pthread.h>
#include "threadPool.h"

#define MAX_LISTEN  128
#define LOCAL_IPADDRESS "172.30.149.120"
#define SERVER_PORT 5555
#define BUFFER_SIZE 128

#define MIN_THREADNUMS  5
#define MAX_THREADNUMS  10
#define MAX_QUEUE       10

// void signalHandler(int sigNum)
// {
//     int ret = 0;
//     /* 回收资源 todo.... */

// }

/* 线程处理函数 */
void * threadHandleFunc(void * arg)
{
    /* 线程分离 */
    pthread_detach(pthread_self());

    int acceptfd = *(int *)arg;

    /* 接收缓冲区 */
    char recvBuffer[BUFFER_SIZE];
    memset(recvBuffer, 0, sizeof(recvBuffer));

    /* 读取字节数 */
    int readBytes = 0;

    /* 发送缓冲区 */
    char sendBuffer[BUFFER_SIZE];
    memset(sendBuffer, 0, sizeof(sendBuffer));

    while (1)
    {
        readBytes = read(acceptfd, recvBuffer, sizeof(recvBuffer) - 1);
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
            printf("buffer : %s\n", recvBuffer);
            sleep(2);

            strncpy(sendBuffer, "一起加油", sizeof(sendBuffer) - 1);
            write(acceptfd, sendBuffer, strlen(sendBuffer));
        }
    }

    pthread_exit(NULL);

}

int main()
{
    // /* 捕捉信号 */
    // signal(SIGINT, signalHandler);
    // signal(SIGQUIT, signalHandler);
    // signal(SIGTSTP, signalHandler);

    /* 初始化线程池 */
    threadPool pool;
    threadPoolInit(&pool, MIN_THREADNUMS, MAX_THREADNUMS, MAX_QUEUE);

    /* 创建套接字 */
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1)
    {
        perror("socket error");
        exit(-1);
    }

    int enableOpt = 1;
    int ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (void *) &enableOpt, sizeof(enableOpt));
    if (ret == -1)
    {
        perror("setsockopt error");
        close(socketfd);
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
    ret = bind(socketfd, (struct sockaddr *) &localAddr, localAddrLen);
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
    
    
    
    while (1)
    {
        /* 循环接收客户端的请求 */
        socklen_t clientAddrLen = 0;
        int acceptfd = accept(socketfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (acceptfd == -1)
        {
            perror("listen error");
            close(socketfd);
            exit(-1);
        }

        /* 像线程池中添加任务到线程池的任务队列 */
        taskQueueInsert(&pool, threadHandleFunc, &clientAddrLen);

#if 0
        /* 通信：多线程 */
        pthread_t tid;
        ret = pthread_create(&tid, NULL, threadHandleFunc, (void *)&acceptfd);
        if (ret != 0)
        {
            perror("pthread create error");
            close(acceptfd);
            close(socketfd);
            exit(1);
        }
#endif
        
    }
    
    /* 销毁线程池 */
    threadPoolDestroy(&pool);
    
    close(socketfd);

    return 0;
}