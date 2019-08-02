
#define __USE_GNU
#include "mod.h"
#include <dlfcn.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include  <unistd.h> //read write

#include <mtcp_api.h>

#include "debug.h"
#include <stdlib.h>

#define MAX_FLOW_NUM  (10000)
#define RCVBUF_SIZE (2*1024)
#define SNDBUF_SIZE (8*1024)

#define MAX_EVENTS (MAX_FLOW_NUM * 3)
#define HTTP_HEADER_LEN 1024
#define NAME_LIMIT 256
#define FD_VALUE 2000

static int (*real_close)(int);
static int (*real_socket)(int,int,int);
static int (*real_bind)(int, const struct sockaddr*, socklen_t);
static int (*real_connect)(int, const struct sockaddr*, socklen_t);
static int (*real_listen)(int, int);
static int (*real_accept)(int, struct sockaddr *, socklen_t *);
static ssize_t (*real_read)(int, void *, size_t );
static ssize_t (*real_write)(int, const void *, size_t );

static int (*real_setsockopt)(int, int, int, const void *, socklen_t);
static int (*real_getsockname)(int , struct sockaddr *, socklen_t *);
static int (*real_getpeername)(int , struct sockaddr *, socklen_t *);

static int (*real_epoll_create)(int);
static int (*real_epoll_ctl)(int, int, int, struct epoll_event *);
static int (*real_epoll_wait)(int, struct epoll_event *, int, int);

static int (*real_fcntl)(int, int,...);

static char *conf_file = "epserver.conf";
static int core_limit;

struct thread_context
{
    mctx_t mctx;
    int ep;
    struct server_vars *svars;
};


struct thread_context *ctx;
mctx_t mctx;

//struct mtcp_epoll_event *events;

int socketnum;
int redis_cpu;
/*struct server_vars
 {
 char request[HTTP_HEADER_LEN];
 int recv_len;
 int request_len;
 long int total_read, total_sent;
 uint8_t done;
 uint8_t rspheader_sent;
 uint8_t keep_alive;
 
 int fidx;                        // file cache index
 char fname[NAME_LIMIT];                // file name
 long int fsize;                    // file size
 }; */
void testprint(){
    printf("*****sucessful test!!!!!ONVM----****\n");
    return;
}

void setconfm(){
    struct mtcp_conf mcfg;
    int ret;
    //struct mtcp_epoll_event ev;
    //struct epoll_event evv;
    core_limit=redis_cpu+1;
    //conf_file = "epserver.conf";
    mtcp_getconf(&mcfg);
    mcfg.num_cores = core_limit;
    mtcp_setconf(&mcfg);
    
    //int listener;
    //struct sockaddr_in saddr;
    
    
    //struct epoll_event *eevents;
    //int nevents;
    //int ep;
    
    //int connfd;
    
    //char buf[HTTP_HEADER_LEN];
    
    //int i;
    
    socketnum = 1;
    
    printf("%s*******",conf_file);
    ret = mtcp_init(conf_file);
    if(ret){
        printf("init fail!");
        return;
    }
    mtcp_getconf(&mcfg);
    
    /*---------------------------*/
    
    mtcp_core_affinitize(redis_cpu);
    ctx = (struct thread_context *)calloc(1, sizeof(struct thread_context));
    ctx->mctx = mtcp_create_context(redis_cpu);
    
    // epoll_create PART
    /*socketnum = 2;
     ctx->ep = epoll_create(MAX_EVENTS);
     if (ctx->ep < 0) {
     mtcp_destroy_context(ctx->mctx);
     free(ctx);
     TRACE_ERROR("Failed to create epoll descriptor!\n");
     return;
     }*/
    
    /* allocate memory for server variables */
    /*->svars = (struct server_vars *)
     calloc(MAX_FLOW_NUM, sizeof(struct server_vars));
     if (!ctx->svars) {
     mtcp_close(ctx->mctx, ctx->ep);
     mtcp_destroy_context(ctx->mctx);
     free(ctx);
     TRACE_ERROR("Failed to create server_vars struct!\n");
     return;
     } */
    
    // socket PART
    /*socketnum = 2;
     listener = socket(AF_INET, SOCK_STREAM, 0);
     if (listener < 0) {
     TRACE_ERROR("Failed to create listening socket!\n");
     return ;
     } */
    
    /* setsockopt PART
     socketnum = 2;
     int opt = 1 ;
     ret = setsockopt(listener, SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt)); */
    
    //setsock_nonblock PART
    /* ret = mtcp_setsock_nonblock(ctx->mctx, listener);
     if (ret < 0) {
     TRACE_ERROR("Failed to set socket in nonblocking mode.\n");
     return ;
     } */
    
    /* bind to port 80 */
    /* saddr.sin_family = AF_INET;
     saddr.sin_addr.s_addr = INADDR_ANY;
     saddr.sin_port = htons(80); */
    
    //bind PART
    /* socketnum = 2;
     ret = bind(listener,(struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
     if (ret < 0) {
     TRACE_ERROR("Failed to bind to the listening socket!\n");
     return ;
     } */
    
    
    /* listen (backlog: can be configured) */
    //listen + epoll_ctl PART
    /*
     socketnum = 2;
     ret = listen(listener, 4096);
     if(ret<0){
     TRACE_ERROR("Failed to start the listening!\n");
     return ;
     }
     
     ev.events = MTCP_EPOLLIN; printf("%d|%d\n",ev.events,(&ev)->events);
     ev.data.sockid = listener;
     evv.events = EPOLLIN;
     evv.data.fd = listener;
     //mtcp_epoll_ctl(ctx->mctx, ctx->ep, MTCP_EPOLL_CTL_ADD, listener, &ev);
     socketnum = 2;
     epoll_ctl(ctx->ep, EPOLL_CTL_ADD, listener, &evv);
     printf("successful listen"); */
    
    /*
     mctx = ctx->mctx;
     ep = ctx->ep;
     events = (struct mtcp_epoll_event *) calloc(MAX_EVENTS, sizeof(struct mtcp_epoll_event));
     eevents = (struct epoll_event *) calloc(MAX_EVENTS, sizeof(struct epoll_event));
     if (!events) {
     TRACE_ERROR("Failed to create event struct!\n");
     exit(-1);
     }
     while(1){
     socketnum = 2;
     nevents = epoll_wait(ep, eevents, MAX_EVENTS, -1);
     if (nevents < 0) {
     if (errno != EINTR)
     perror("mtcp_epoll_wait");
     break;
     }
     for (i = 0; i < nevents; i++) {
     */
    //accept PART
    /*
     if (events[i].data.sockid == listener) {
     socketnum = 2;
     connfd = accept(listener, NULL, NULL);
     if (connfd < 0)
     {
     perror("accept error");
     continue;
     }
     if (connfd >= 0) {
     if (connfd >= MAX_FLOW_NUM) {
     TRACE_ERROR("Invalid socket id %d.\n", connfd);
     return ;
     }
     
     
     ev.events = MTCP_EPOLLIN;
     ev.data.sockid = connfd;
     evv.events = EPOLLIN;
     evv.data.fd = connfd;
     mtcp_setsock_nonblock(ctx->mctx, connfd);
     socketnum = 2;
     epoll_ctl(ctx->ep, EPOLL_CTL_ADD, connfd, &evv);
     TRACE_APP("Socket %d registered.\n", connfd);
     
     }else {
     if (errno != EAGAIN) {
     TRACE_ERROR("mtcp_accept() error %s\n",
     strerror(errno));
     }
     }
     
     } */
    //read PART
    /*
     else if (events[i].events & MTCP_EPOLLIN) {
     socketnum = 2;
     ret = read(events[i].data.sockid, buf, HTTP_HEADER_LEN);
     
     if (ret == 0) {
     // connection closed by remote host
     mtcp_epoll_ctl(ctx->mctx, ctx->ep, MTCP_EPOLL_CTL_DEL, events[i].data.sockid, NULL);
     mtcp_close(ctx->mctx, events[i].data.sockid);
     } else if (ret < 0) {
     // if not EAGAIN, it's an error
     if (errno != EAGAIN) {
     mtcp_epoll_ctl(ctx->mctx, ctx->ep, MTCP_EPOLL_CTL_DEL, events[i].data.sockid, NULL);
     mtcp_close(ctx->mctx, events[i].data.sockid);
     }
     }
     else {
     printf("%s!!!\n",buf);
     int rwrite;
     buf[1]='A';
     socketnum = 2;
     rwrite = write(events[i].data.sockid,buf,ret);
     printf("write %d %s\n",rwrite,buf);
     if(rwrite<0){ret=rwrite;}
     //mtcp_epoll_ctl(ctx->mctx, ctx->ep, MTCP_EPOLL_CTL_DEL, events[i].data.sockid, NULL);
     //mtcp_close(ctx->mctx, events[i].data.sockid);
     }
     
     } */
    
    //}  //for
    //}   //while
    /*
     mtcp_destroy_context(mctx); */
    return ;
    
}
/*
void setclient(){
    struct mtcp_conf mcfg;
    int ret;

    core_limit=1;
    mtcp_getconf(&mcfg);
    mcfg.num_cores = core_limit;
    mtcp_setconf(&mcfg);
    
    socketnum = 1;
    
    printf("%s*******",conf_file);
    ret = mtcp_init(conf_file);
    if(ret){
        printf("init fail!");
        return;
    }
    mtcp_getconf(&mcfg);
    
    
    
    mtcp_core_affinitize(0);
    ctx = (struct thread_context *)calloc(1, sizeof(struct thread_context));
    ctx->mctx = mtcp_create_context(0);
    
    
    return ;
    
}
*/
void mod(){
#define INIT_FUNCTION(func) \
real_##func = dlsym(RTLD_NEXT, #func); \
assert(real_##func);
    
    INIT_FUNCTION(close);
    INIT_FUNCTION(socket);
    INIT_FUNCTION(bind);
    INIT_FUNCTION(connect);
    INIT_FUNCTION(listen);
    INIT_FUNCTION(accept);
    INIT_FUNCTION(read);
    INIT_FUNCTION(write);
    
    INIT_FUNCTION(setsockopt);
    INIT_FUNCTION(getsockname);
    INIT_FUNCTION(getpeername);
    
    INIT_FUNCTION(epoll_create);
    INIT_FUNCTION(epoll_ctl);
    INIT_FUNCTION(epoll_wait);
    

    INIT_FUNCTION(fcntl);    
#undef INIT_FUNCTION
    
    
    return;
}


void destroy_nf(){
    destroy_mtcp();
    mtcp_destroy();
    return;
}


void destroy_mtcp(){
    return mtcp_destroy_context(mctx);
}
int setnonblocking(int sockfd){
    if (sockfd>FD_VALUE){
        sockfd-=FD_VALUE;
        return mtcp_setsock_nonblock(ctx->mctx,sockfd);
    }
    else return -1;
    
}

int close(int fd){
    if (fd>FD_VALUE){
        fd-=FD_VALUE;
        return mtcp_close(ctx->mctx, fd);
    }
    return real_close(fd);
}

int socket(int domain, int type, int protocol){
    
    if (socketnum == 2){
        socketnum = 1;
        int fd = mtcp_socket(ctx->mctx,domain,type,protocol);
        if(fd>0)
            fd+= FD_VALUE;
        printf("mtcpsocket run\n");
        return fd;
    }
    else if (socketnum == 3){
        socketnum = 1;
        int fd = mtcp_socket(ctx->mctx,domain,type,protocol);
        int ret = mtcp_setsock_nonblock(ctx->mctx, fd);
        if(fd>0)
            fd+= FD_VALUE;
        printf("mtcpsocket connect run %d \n",ret);
        return fd;
    }
    else return real_socket(domain,type,protocol);
}
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    if(sockfd>FD_VALUE){
        sockfd-=FD_VALUE;
        return mtcp_bind(ctx->mctx,sockfd, addr, addrlen);}
    else return real_bind(sockfd,addr,addrlen);
}
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    if (sockfd >FD_VALUE)
    {
        sockfd -= FD_VALUE;
        printf("mtcpconnect run\n");
        return mtcp_connect(ctx->mctx,sockfd, addr, addrlen);
    }
    else
    {
        return real_connect(sockfd, addr, addrlen);
    }
    
}

int listen(int sockfd, int backlog){
    if(sockfd>FD_VALUE){
        sockfd-=FD_VALUE;
        return mtcp_listen(ctx->mctx,sockfd,backlog);}
    else return real_listen(sockfd,backlog);
}
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    int f;
    if(sockfd>FD_VALUE){
        sockfd-=FD_VALUE;
        f = mtcp_accept(ctx->mctx,sockfd,addr,addrlen);
        addr->sa_family = AF_INET;
        if(f>0)
            f+=FD_VALUE;
        //printf("acceptfdout=%d\n",f);
        return f;}
    else return accept(sockfd,addr,addrlen);
}
ssize_t read(int fd, void *buf, size_t count){
    if(fd>FD_VALUE){
        fd-=FD_VALUE;
        return mtcp_read(ctx->mctx,fd,buf,count);}
    else return real_read(fd,buf,count);
}
ssize_t write(int fd, const void *buf, size_t count){
    if(fd>FD_VALUE){
        fd-=FD_VALUE;
        return mtcp_write(ctx->mctx,fd,buf,count);}
    else return real_write(fd,buf,count);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen){
    if(sockfd>FD_VALUE){
        sockfd-=FD_VALUE;
        return mtcp_setsockopt(ctx->mctx,sockfd, level, optname, optval, optlen);}
    else return real_setsockopt(sockfd, level, optname, optval, optlen);
}

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    if(sockfd>FD_VALUE){
        sockfd-=FD_VALUE;
        return mtcp_getsockname(ctx->mctx,sockfd,addr,addrlen);}
    else return real_getsockname(sockfd,addr,addrlen);
}

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    if(sockfd>FD_VALUE){
        sockfd-=FD_VALUE;
        return mtcp_getpeername(ctx->mctx,sockfd, addr, addrlen);}
    else return real_getpeername(sockfd, addr, addrlen);
}


int epoll_create (int size){
    if(socketnum==2){
        socketnum = 1;
        ctx->ep=mtcp_epoll_create(ctx->mctx,size);
        return ctx->ep+FD_VALUE;}
    else return real_epoll_create(size);
}
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event){
    struct mtcp_epoll_event temp;
    if(epfd>=FD_VALUE){
        if(fd<=FD_VALUE){
            printf("skip linuxfd\n");
            return 0;
        }
        epfd-=FD_VALUE;
        fd-=FD_VALUE;
        temp.events = event->events;
        temp.data.sockid = fd;
        ctx->ep = epfd;
        return mtcp_epoll_ctl(ctx->mctx,epfd,op,fd,&temp);
    }
    else {
        if(fd>FD_VALUE)
        {
            printf("skip mtcp");
            return 0;
        }
        return real_epoll_ctl(epfd,op,fd,event);
    }
}
int epoll_wait(int epfd, struct epoll_event *eventss, int maxevents, int timeout){
    //struct mtcp_epoll_event *temp;
    //temp = (struct mtcp_epoll_event *) calloc(MAX_EVENTS, sizeof(struct mtcp_epoll_event));
    static struct mtcp_epoll_event temp[MAX_EVENTS*sizeof(struct mtcp_epoll_event)];
    static int result,i;
    if(epfd>=FD_VALUE){
        //temp = (struct mtcp_epoll_event *) malloc(MAX_EVENTS*sizeof(struct mtcp_epoll_event));
        epfd-=FD_VALUE;
        mctx = ctx->mctx;
        result = mtcp_epoll_wait(mctx,epfd, temp, maxevents, timeout);
        for(i=0;i<result;i++){
            eventss[i].events = temp[i].events;
            eventss[i].data.fd = temp[i].data.sockid + FD_VALUE;
        }
        //free(temp);
        return result;
    }
    else return real_epoll_wait(epfd, eventss, maxevents, timeout);
}


//int main(){mod();return 0;}
int fcntl(int fd, int cmd,...){
    if(fd>=FD_VALUE){
        fd-=2000;
        return mtcp_setsock_nonblock(ctx->mctx,fd);
    }
    return real_fcntl(fd,cmd);
}

