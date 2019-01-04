#include  <unistd.h>
#include  <sys/types.h>       /* basic system data types */
#include  <sys/socket.h>      /* basic socket definitions */
#include  <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include  <arpa/inet.h>       /* inet(3) functions */
#include <sys/epoll.h> /* epoll function */
#include <fcntl.h>     /* nonblocking */
#include <sys/resource.h> /*setrlimit */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <mtcp_api.h>
#include <mtcp_epoll.h>

#include "mod.h"


#define MAXEPOLLSIZE 30000
#define MAXLINE 1024

extern int socketnum;
extern mctx_t mctx;

int handle(int connfd);


int main(int argc, char **argv)
{
    int  servPort = 80;
    int listenq = 4096;
    
    int listenfd, connfd, kdpfd, nfds, n, curfds,acceptCount;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    struct epoll_event ev;
    struct epoll_event *events;
    //struct mtcp_epoll_event *mevents;
    struct rlimit rt;
    char buf[MAXLINE];
    acceptCount = 0;
    
    mod();
    setconfm();
    
    /* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1)
    {
        perror("setrlimit error");
        return -1;
    }
    
    socketnum = 2;
    kdpfd = epoll_create(MAXEPOLLSIZE);
    printf("%d\n",kdpfd);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons (servPort);
    
    socketnum = 2;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("can't create socket file");
        return -1;
    }
    printf("listenfd=%d\n",listenfd);
    
    int opt = 1;
    socketnum = 2;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (setnonblocking(listenfd) < 0) {
        perror("setnonblock error");
    }
    
    socketnum = 2;
    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind error");
        return -1;
    }
    
    socketnum = 2;
    if (listen(listenfd, listenq) == -1)
    {
        perror("listen error");
        return -1;
    }
    /* 创建 epoll 句柄，把监听 socket 加入到 epoll 集合里 */
    
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    socketnum = 2;
    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
    {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", listenfd);
        return -1;
    }
    curfds = 1;
    
    printf("epollserver startup,port %d, max connection is %d, backlog is %d\n", servPort, MAXEPOLLSIZE, listenq);
    
    events = (struct epoll_event *) calloc(MAXEPOLLSIZE, sizeof(struct epoll_event));
    //mevents = (struct mtcp_epoll_event *) calloc(MAXEPOLLSIZE, sizeof(struct mtcp_epoll_event));
    
    while(1) {
        /* 等待有事件发生 */
        socketnum = 2;
        nfds = epoll_wait(kdpfd, events, MAXEPOLLSIZE, -1);
        if (nfds < 0) {
            if (errno != EINTR)
                perror("mtcp_epoll_wait");
            break;
        }
        /* 处理所有事件 */
        for (n = 0; n < nfds; ++n)
        {
            //events[n].events = mevents[n].events;
            //events[n].data.fd = mevents[n].data.sockid;
            if (events[n].data.fd == listenfd)
            {
                socketnum = 2;
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr,&socklen);
                if (connfd < 0)
                {
                    perror("accept error");
                    continue;
                }
                
                sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
                printf("%d:%s", ++acceptCount, buf);
                
                if (curfds >= MAXEPOLLSIZE) {
                    fprintf(stderr, "too many connection, more than %d\n", MAXEPOLLSIZE);
                    socketnum = 2;
                    close(connfd);
                    continue;
                }
                if (setnonblocking(connfd) < 0) {
                    perror("setnonblocking error");
                }
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                socketnum = 2;
                if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, connfd, &ev) < 0)
                {
                    fprintf(stderr, "add socket '%d' to epoll failed: %s\n", connfd, strerror(errno));
                    return -1;
                }
                curfds++;
                continue;
            }
            // 处理客户端请求
            if (handle(events[n].data.fd) < 0) {
                socketnum = 2;
                epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd,&ev);
                curfds--;
                
            }
        }
    }
    
    socketnum = 2;
    close(listenfd);
    destroy_mtcp();
    return 0;
    
}
int handle(int connfd) {
    int nread,nwrite;
    char buf[MAXLINE];
    socketnum = 2;
    nread = read(connfd, buf, MAXLINE);//读取客户端socket流
    
    if (nread == 0) {
        printf("client close the connection\n");
        socketnum = 2;
        close(connfd);
        return -1;
    }
    if (nread < 0) {
        perror("read error");
        socketnum = 2;
        close(connfd);
        return -1;
    }
    socketnum = 2;
    nwrite = write(connfd, buf, nread);//响应客户端
    if(nwrite<0)
    {
        perror("write error");
    }
    return 0;
}



