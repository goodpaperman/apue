#include <stdarg.h>   
#include <errno.h>   
#include <stdio.h>   
#include <fcntl.h>   
#include <unistd.h>   
#include <string.h>   
#include <time.h>   
#include <sys/types.h>   
#include <sys/stat.h>   
#include <dirent.h>   
#include <errno.h>   
#include <netinet/in.h>   
#include <sys/socket.h>   
#include <resolv.h>   
#include <arpa/inet.h>   
#include <stdlib.h>   
#include <signal.h>   
#include <getopt.h>   
#include <pthread.h>   
#include <strings.h> 
#include "pty_fun.h"   
  
#define DEFAULTIP         "127.0.0.1"   
#define DEFAULTPORT       "23" //"20013"   
#define DEFAULTBACK       "10"   
#define DEFAULTDIR        "~" //"/root"
#define DEFAULTLOG        "/tmp/telnet-server.log"   
#define PTY_NAME_SIZE     20   
#define MAX_BUFSIZE       512   
  
typedef struct thread_arg  
{  
    int pty;  
    int sockfd;  
} thread_arg;  
  
void prterrmsg(char *msg);  
#define prterrmsg(msg)        { perror(msg); abort(); }   
void wrterrmsg(char *msg);  
#define wrterrmsg(msg)        { fputs(msg, logfp); fputs(strerror(errno), logfp);fflush(logfp); abort(); }   
  
void prtinfomsg(char *msg);  
#define prtinfomsg(msg)        { fputs(msg, stdout);  }   
void wrtinfomsg(char *msg);  
#define wrtinfomsg(msg)        {  fputs(msg, logfp); fflush(logfp);}   
  
#define MAXBUF        1024   
  
char *host = 0;  
char *port = 0;  
char *back = 0;  
char *dirroot = 0;  
char *logdir = 0;  
unsigned char daemon_y_n = 0;  
FILE *logfp;  
  
#define MAXPATH        150   
  
/*------------------------------------------------------ 
 *--- AllocateMemory - 分配空间并把d所指的内容复制 
 *------------------------------------------------------ 
 */  
void AllocateMemory(char **s, int l, char *d)  
{  
    *s = malloc(l + 1);  
    bzero(*s, l + 1);  
    memcpy(*s, d, l);  
}  
  
/*------------------------------------------------------ 
 *--- getoption - 分析取出程序的参数 
 *------------------------------------------------------ 
 */  
void getoption(int argc, char **argv)  
{  
    int c = 0, len = 0;  
    char *p = 0;  
  
    opterr = 0;  
    while (1)  
    {  
        int option_index = 0;  
        static struct option long_options[] =  
        {  
        { "host", 1, 0, 0 },  
        { "port", 1, 0, 0 },  
        { "back", 1, 0, 0 },  
        { "dir", 1, 0, 0 },  
        { "log", 1, 0, 0 },  
        { "daemon", 0, 0, 0 },  
        { 0, 0, 0, 0 } };  
        /* 本程序支持如一些参数： 
         * --host IP地址 或者 -H IP地址 
         * --port 端口 或者 -P 端口 
         * --back 监听数量 或者 -B 监听数量 
         * --dir 服务默认目录 或者 -D 服务默认目录 
         * --log 日志存放路径 或者 -L 日志存放路径 
         * --daemon 使程序进入后台运行模式 
         */  
        c = getopt_long(argc, argv, "H:P:B:D:L", long_options, &option_index);  
        if (c == -1 || c == '?')  
            break;  
  
        if (optarg)  
            len = strlen(optarg);  
        else  
            len = 0;  
  
        if ((!c && !(strcasecmp(long_options[option_index].name, "host"))) || c == 'H')  
            p = host = malloc(len + 1);  
        else if ((!c && !(strcasecmp(long_options[option_index].name, "port"))) || c == 'P')  
            p = port = malloc(len + 1);  
        else if ((!c && !(strcasecmp(long_options[option_index].name, "back")))  || c == 'B')  
            p = back = malloc(len + 1);  
        else if ((!c && !(strcasecmp(long_options[option_index].name, "dir"))) || c == 'D')  
            p = dirroot = malloc(len + 1);  
        else if ((!c && !(strcasecmp(long_options[option_index].name, "log"))) || c == 'L')  
            p = logdir = malloc(len + 1);  
        else if ((!c && !(strcasecmp(long_options[option_index].name, "daemon"))))  
        {  
            daemon_y_n = 1;  
            continue;  
        }  
        else  
            break;  

        bzero(p, len + 1);  
        memcpy(p, optarg, len);  
    }  
}  
  
//void *thread_recv_add_write_pyt(void *arg)  
//{  
//    int ret, sockfd, pty;  
//    char buf[MAX_BUFSIZE];  
//    thread_arg *arg1 = (thread_arg*) arg;  
//  
//    sockfd = arg1->sockfd;  
//    pty = arg1->pty;  
//  
//    while (1)  
//    {  
//        memset(buf, 0, MAX_BUFSIZE);  
//        ret = recv(sockfd, buf, MAX_BUFSIZE, 0);  
//        if (ret < 0)  
//        {  
//            continue;  
//        }  
//        printf("%s", buf);  
//        write(pty, buf, strlen(buf));  
//    }  
//}  
  
void read_write_pty(int pty, int sockfd)  
{  
    char buf[MAX_BUFSIZE] = { 0 };  
    int ret = 0;  
    //  pthread_t thread_t;   
    //  pthread_attr_t attr;   
    //  thread_arg arg;   
    //  arg.pty = pty;   
    //  arg.sockfd = sockfd;   
    //   
    //  if ((ret = pthread_create(&thread_t, &attr, thread_recv_add_write_pyt,   
    //          (void*) &arg)))   
    //  {   
    //      perror("thread create recieve message and write to pty err:\n");   
    //  }   
  
    pid_t pid = 0;  
    if ((pid = fork()) < 0)  
    {  
        perror("");  
        exit(1);  
    }  
    else if (pid == 0)  
    {  
        while (1)  
        {  
            memset(buf, 0, MAX_BUFSIZE);  
            ret = recv(sockfd, buf, MAX_BUFSIZE - 1, 0);  
            if (ret < 0)  
            {  
                //continue;  
                exit (ret); 
            }  
            //printf("%s", buf);   
            write(pty, buf, strlen(buf));  
        }  
    }  
  
    memset(buf, 0, MAX_BUFSIZE);  
    while ((ret = read(pty, buf, MAX_BUFSIZE - 1)))
    {  
        buf[ret - 1] = 0; //read是读不到字符串结束符的，需要自己添加，否则printf会出错   
        if (ret <= 0)  
        {  
            //break;  
            exit (ret); 
        }  

        //fflush(stdout);//这步很重要，std中经常有数据滞留在存储区中需要此函数刷新   
        //printf("%s", buf);//打印出结果   
        send(sockfd, buf, strlen(buf), 0);  
        memset(buf, 0, MAX_BUFSIZE);  
    }  
    return;  
}  
  
int main(int argc, char **argv)  
{  
    struct sockaddr_in addr;  
    int sock_fd = 0; 
    socklen_t addrlen = 0;  
  
    /* 获得程序工作的参数，如 IP 、端口、监听数、网页根目录、目录存放位置等 */  
    getoption(argc, argv);  
  
    if (!host)  
    {  
        addrlen = strlen(DEFAULTIP);  
        AllocateMemory(&host, addrlen, DEFAULTIP);  
    }  
    if (!port)  
    {  
        addrlen = strlen(DEFAULTPORT);  
        AllocateMemory(&port, addrlen, DEFAULTPORT);  
    }  
    if (!back)  
    {  
        addrlen = strlen(DEFAULTBACK);  
        AllocateMemory(&back, addrlen, DEFAULTBACK);  
    }  
    if (!dirroot)  
    {  
        addrlen = strlen(DEFAULTDIR);  
        AllocateMemory(&dirroot, addrlen, DEFAULTDIR);  
    }  
    if (!logdir)  
    {  
        addrlen = strlen(DEFAULTLOG);  
        AllocateMemory(&logdir, addrlen, DEFAULTLOG);  
    }  
  
    /* fork() 两次处于后台工作模式下 */  
    if (daemon_y_n)  
    {  
        if (fork())  
            exit(0);  
        if (fork())  
            exit(0);  
        close(0), close(1), close(2);  
        logfp = fopen(logdir, "a+");  
        if (!logfp)  
            exit(0);  
    }  
  
    /* 处理子进程退出以免产生僵尸进程 */  
    signal(SIGCHLD, SIG_IGN);  
  
    /* 创建 socket */  
    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  
    {  
        if (!daemon_y_n)  
        {  
            prterrmsg("socket()");  
        }  
        else  
        {  
            wrterrmsg("socket()");  
        }  
    }  
  
    addrlen = 1; 
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &addrlen, sizeof(addrlen));  
    /* 设置端口快速重用 */  
    addr.sin_family = AF_INET;  
    addr.sin_port = htons(atoi(port));  
    // should use 'host' instead of INADDR_ANY
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  
  
    /* 绑定地址、端口等信息 */  
    addrlen = sizeof(struct sockaddr_in);  
    if (bind(sock_fd, (struct sockaddr *) &addr, addrlen) < 0)  
    {  
        if (!daemon_y_n)  
        {  
            prterrmsg("bind()");  
        }  
        else  
        {  
            wrterrmsg("bind()");  
        }  
    }  
  
    /* 开启临听 */  
    if (listen(sock_fd, atoi(back)) < 0)  
    {  
        if (!daemon_y_n)  
        {  
            prterrmsg("listen()");  
        }  
        else  
        {  
            wrterrmsg("listen()");  
        }  
    }  
  
    printf("host=%s port=%s back=%s dirroot=%s logdir=%s %s是后台工作模式(进程ID：%d)\n",  
            host, port, back, dirroot, logdir, daemon_y_n ? "" : "不", getpid());  
  
    while (1)  
    {  
        int new_fd = 0;  
        addrlen = sizeof(struct sockaddr_in);  
        /* 接受新连接请求 */  
        new_fd = accept(sock_fd, (struct sockaddr *) &addr, &addrlen);  
        if (new_fd < 0)  
        {  
            if (!daemon_y_n)  
            {  
                prterrmsg("accept()");  
            }  
            else  
            {  
                wrterrmsg("accept()");  
            }  
            break;  
        }  

        char buffer[MAXBUF + 1];  
        bzero(buffer, MAXBUF + 1);  
        sprintf(buffer, "连接来自于: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));  
        if (!daemon_y_n)  
        {  
            prtinfomsg(buffer);  
        }  
        else  
        {  
            wrtinfomsg(buffer);  
        }  
        /* 产生一个子进程去处理请求，当前进程继续等待新的连接到来 */  
        if (!fork())  
        {  
            int ret = -1, ptrfdm = -1;  
            char slave_name[PTY_NAME_SIZE] = { 0 };  
            struct termios slave_termiors;  
            struct winsize slave_winsize;  
            pid_t ppid = -1;  
  
            //ret = -1;  
            //ptrfdm = -1;  
            //memset(slave_name, 0, PTY_NAME_SIZE);  
            //ppid = -1;  
  
            ret = pty_fork(&ptrfdm, slave_name, PTY_NAME_SIZE, &slave_termiors,  
                    &slave_winsize, &ppid);  
  
            if (ret < 0)  
            {  
                printf("pty_fork err ! ret = %d", ret);  
                return -1;  
            }  
  
            if (ppid < 0)  
            {  
                printf("pty_fork err !");  
                return -1;  
            }  
            else if (ppid == 0)  
            {  
                execl("/bin/bash", "bash", NULL);  
            }  
            else  
            {  
                read_write_pty(ptrfdm, new_fd);  
  
            }  
        }  
        close(new_fd);  
    }  
    close(sock_fd);  
    return 0;  
}  
