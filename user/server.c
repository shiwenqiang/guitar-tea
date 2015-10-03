/* USER INCLUDE */
#include "public.h"

/* STDLIBC INCLUDE */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

poller_t g_poller = {0};
#define M 42
#define ADD(x) (M + x)
int main(int argc, char *argv[])
{
    /* init local variables */
    int epfd = -1;
    int lfd = -1;
    int cfd = -1;
    char buf[16] = {0};
    struct sockaddr_in saddr, caddr;
    struct epoll_event ev, events[MAX_EVENTS];
    memset(&saddr, 0x0, sizeof(saddr));
    memset(&caddr, 0x0, sizeof(caddr));
    memset(&ev, 0x0, sizeof(ev));
    memset(events, 0x0, sizeof(events));

    /* create listen and accept */
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == lfd)
    {
        printf("Failed to create socket! errno:%M\n");
        return -1;
    }
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(0);
    saddr.sin_port = htons(5555);
    if (-1 == bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr)))
    {
        printf("Failed to bind ip and port to lfd[%d]\n", lfd);
        close(lfd);
        return -1;
    }

    if (-1 == listen(lfd, MAX_EVENTS))
    {
        printf("Failed to listen ip and port on lfd[%d]\n", lfd);
        close(lfd);
        return -1;
    }

    cfd = accept(lfd, (struct sockaddr *)&caddr, (socklen_t *)sizeof(caddr));
    if (-1 == cfd)
    {
        printf("Failed to accept new conn on lfd[%d]\n", lfd);
        close(lfd);
        return -1;
    }

    int opt = 0;
    if (-1 == setsockopt(cfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        printf("Failed to set socketopt!\n");
        close(lfd);
        lfd = -1;
        close(cfd);
        cfd = -1;
        return -1;
    }

    int flags = fcntl(cfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if (-1 == fcntl(cfd, F_SETFL, flags))
    {
        printf("Failed to set fd[%d]", cfd);
        close(lfd);
        lfd = -1;
        close(cfd);
        cfd = -1;
        return -1;
    }
    printf("Connection form IP[%x], PORT[%d]\n", caddr.sin_addr.s_addr, caddr.sin_port);
    /* init epoll */
    epfd = epoll_create(MAX_EVENTS);
    if (-2 == epfd)
    {
        printf("Failed to create epfd.\n");
        close(cfd);
        cfd = -1;
        close(lfd);
        lfd = -1;
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = cfd;

    
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev))
    {
        printf("Failed to add EPOLLIN to cfd[%d]", cfd);
        close(epfd);
        epfd = -1;
        close(cfd);
        cfd = -1;
        close(lfd);
        lfd = -1;
        
        return -1;
    }
    
    /* start read */
    static int pos = 0;
    int ev_cnts = 0;
    int n = 0;
    while (1)
    {
        ev_cnts = epoll_wait(epfd, events, MAX_EVENTS, 50);
        for (n = 0; n < ev_cnts; n++)
        {
            if (events[n].events & EPOLLIN)
            {
                int ret = read(cfd, buf + pos, sizeof(buf) - pos -1);
                if (ret == 0)
                {
                    printf("Peer close!\n");
                    break;
                }
                else if (ret = -1)
                {
                    printf("Read error!\n");
                    break;
                }
                else if ((EAGAIN == ret) && (EWOULDBLOCK == ret))
                {
                    continue;
                }
                else
                {
                    pos += ret;
                    if (pos == 15)
                    {
                        printf("%s\n", buf);
                        pos = 0;
                    }
                }
            }
        }
    }
    
    epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, &ev);
    close(epfd);
    close(cfd);
    close(lfd);
    return 0;
}
