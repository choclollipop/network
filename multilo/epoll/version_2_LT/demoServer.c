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
#include <sys/epoll.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_LISTEN  128
#define LOCAL_IPADDRESS "172.30.149.120"
#define SERVER_PORT 5556
#define BUFFER_SIZE 128

#define READ_BUFFER 5

int main()
{

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

    int enableOpt = 1;
    int ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (void *) &enableOpt, sizeof(enableOpt));
    if (ret == -1)
    {
        perror("setsockopt error");
        close(socketfd);
        exit(-1);
    }

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

    /* 创建epoll 红黑树实例 */
    int epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        perror("epoll error");
        close(socketfd);
        exit(-1);
    }

    /* 将socket 添加到实例中 */
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = socketfd;
    event.events = EPOLLIN;    //读事件

    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event);
    if (ret == -1)
    {
        perror("epoll_ctl error");
        close(epollfd);
        close(socketfd);
        exit(-1);
    }

    /* 监听返回发生事件的套接字数量 */
    int nums = 0;
    /* 读到的字节数 */
    ssize_t readBytes = 0;
    while (1)
    {
        struct epoll_event events[BUFFER_SIZE];
        memset(events, 0, sizeof(event));
        nums = epoll_wait(epollfd, events, BUFFER_SIZE, -1);
        if (nums == -1)
        {
            perror("epoll_wait error");
            close(epollfd);
            close(socketfd);
            exit(-1);
        }

        printf("nums:%d\n", nums);
        /*
            程序此时剩余两种情况：
            1.超时
            2.有监听事件发生
        */

       for (int idx = 0; idx < nums; idx++)
       {
            int fd = events[idx].data.fd;
            if (fd == socketfd)
            {
                /* 有连接 */
                int connfd = accept(socketfd, NULL, NULL);
                if (connfd == -1)
                {
                    perror("accept error");
                    exit(-1);
                }

                /* 将通信句柄设置为非阻塞 */
                int flag = fcntl(connfd, F_GETFL);
                fcntl(connfd, F_SETFL, flag | O_NONBLOCK);

                /* 将通信套接字添加到树中 */
                struct epoll_event con_event;
                memset(&con_event, 0, sizeof(con_event));
                event.data.fd = connfd;
                event.events = EPOLLIN;

                ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event);
                if (ret == -1)
                {
                    perror("epoll_ctl error");
                    continue;
                }
            }
            else
            {
                char buffer[READ_BUFFER] = {0};
                /* 有通信 */
                readBytes = read(fd, buffer, sizeof(buffer) - 1);
                if (readBytes < 0)
                {
                    perror("read error");
                    /* 将该文件句柄从红黑树上删除 */
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                    /* 关闭通信句柄 */
                    close(fd);
                }
                else if (readBytes == 0)
                {
                    printf("客户端下线......\n");
                    /* 将该文件句柄从红黑树上删除 */
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                    /* 关闭通信句柄 */
                    close(fd);
                }
                else
                {
                    printf("recv : %s\n", buffer);

                    for (int jdx = 0; jdx < readBytes; jdx++)
                    {
                        buffer[jdx] = toupper(buffer[jdx]);
                    }

                    write(fd, buffer, readBytes);

                    usleep(300);
                }
            }
       }
    }
    
#if 0

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

#endif

    close(socketfd);

    return 0;
}