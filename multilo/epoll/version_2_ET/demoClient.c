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

#define SERVER_PORT 5556
#define SERVER_IP   "172.30.149.120"
#define BUFFER_SIZE 128

void * thread_func(void * arg)
{
    pthread_detach(pthread_self());

    char recvBuffer[BUFFER_SIZE];
    

    int socketfd = *(int *)arg;

    while (1)
    {
        memset(recvBuffer, 0, sizeof(recvBuffer));

        int readBytes = read(socketfd, recvBuffer, sizeof(recvBuffer) - 1);
        if (readBytes < 0)
        {

        }
        else if (readBytes == 0)
        {

        }
        else
        {
            printf("recvBuffer : %s\n", recvBuffer);
        }
    }

    pthread_exit(NULL);
}

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

    pthread_t tid;
    ret = pthread_create(&tid, NULL, thread_func, (void *)&socketfd);
    if (ret == -1)
    {
        perror("pthread_create error");
        exit(-1);
    }

    while (1)
    {
        scanf("%s", buffer);
        write(socketfd, buffer, strlen(buffer) + 1);

    }
    

    sleep(5);

    close(socketfd);

    return 0;
}