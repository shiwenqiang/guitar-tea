/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_network.c
 * Module   :Network Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the network.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
/* USER INCLUDE */
#include "public.h"
#include "gt_pub.h"
#include "gt_log.h"

/* STDLIBC INCLUDE */
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

uint32_t g_gt_server_ip = 0;
uint16_t g_gt_server_port = 0;

int32_t gt_create_pipefd(int32_t pipefd[2])
{
    int32_t ret = socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create socketpair pipefd! errno:%d, %s", errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}

int32_t gt_set_nonblock(int32_t fd)
{
    int32_t ret = GT_OK;
    int32_t flag = fcntl(fd, F_GETFL);
    ret = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set fd[%d] to nonblock! errno:%d, %s", fd, errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}

int32_t gt_set_cloexec(int32_t fd)
{
    int32_t ret = GT_OK;
    int32_t flag = fcntl(fd, F_GETFD);
    ret = fcntl(fd, F_SETFD, flag | FD_CLOEXEC);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set fd[%d] to close-on-exec! errno:%d, %s", fd, errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}



#if 0
poller_t g_poller = {0};



int gt_network(int argc, char *argv[])
{
    /* init local variables */
    int epfd = -1;
    int lfd = -1;
    int cfd = -1;
    char buf[1024] = {0};
    struct sockaddr_in saddr, caddr;
    struct epoll_event ev, events[MAX_EVENTS];
    memset(&saddr, 0x0, sizeof(saddr));
    memset(&caddr, 0x0, sizeof(caddr));
    memset(&ev, 0x0, sizeof(ev));
    memset(events, 0x0, sizeof(events));

    /* Parse Input arguments */
    if (argc < 3)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Invalid Parameters. Usage: %s [ip] [port]", argv[0]);
        return GT_ERROR;
    }

    const char *ip = argv[0];
    int32_t port = atoi(argv[2]);

    GT_INFO_LOG(GT_MOD_CORE, "Start listenning...");
    /* create listen and accept */
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 > lfd)
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create socket! errno:%d, %s", tmp_errno, strerror(tmp_errno));
        return GT_ERROR;
    }
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &saddr.sin_addr);
    if (-1 == bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr)))
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to bind ip and port to lfd[%d]! errno:%d, %s", lfd, tmp_errno, strerror(tmp_errno));
        close(lfd);
        return GT_ERROR;
    }

    if (-1 == listen(lfd, MAX_EVENTS))
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to listen ip and port on lfd[%d]! errno:%d, %s", lfd, tmp_errno, strerror(tmp_errno));
        close(lfd);
        return -1;
    }

    socklen_t caddrlen = sizeof(caddr);
    cfd = accept(lfd, (struct sockaddr *)&caddr, &caddrlen);
    if (-1 == cfd)
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to accept new conn on lfd[%d]! errno:%d, %s", lfd, tmp_errno, strerror(tmp_errno));
        close(lfd);
        return -1;
    }

    int opt = 0;
    if (-1 == setsockopt(cfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set socketopt! errno:%d, %s", tmp_errno, strerror(tmp_errno));
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
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set cfd[%d]! errno:%d, %s", cfd, tmp_errno, strerror(tmp_errno));
        close(lfd);
        lfd = -1;
        close(cfd);
        cfd = -1;
        return -1;
    }

    GT_INFO_LOG(GT_MOD_CORE, "Connection form IP[%#x], PORT[%d]", ntohl(caddr.sin_addr.s_addr), ntohs(caddr.sin_port));
    /* init epoll */
    epfd = epoll_create(MAX_EVENTS);
    if (-1 == epfd)
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create epfd. errno:%d, %s", tmp_errno, strerror(tmp_errno));
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
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to add EPOLLIN to cfd[%d], errno:%d, %s", cfd, tmp_errno, strerror(tmp_errno));
        close(epfd);
        epfd = -1;
        close(cfd);
        cfd = -1;
        close(lfd);
        lfd = -1;

        return -1;
    }

    /* start read */
    static int32_t pos = 0;
    int32_t ev_cnts = 0;
    int32_t n = 0;
    while (1)
    {
        ev_cnts = epoll_wait(epfd, events, MAX_EVENTS, 50);
        for (n = 0; n < ev_cnts; n++)
        {
            if (events[n].events & EPOLLIN)
            {
                //int ret = read(cfd, buf + pos, sizeof(buf) - pos -1);
                int ret = read(cfd, buf, sizeof(buf) - pos - 1);
                if (ret == 0)
                {
                    int32_t tmp_errno = errno;
                    GT_ERROR_LOG(GT_MOD_CORE, "Peer close! errno:%d, %s", tmp_errno, strerror(tmp_errno));
                    goto out;
                }
                else if (ret == -1)
                {
                    int32_t tmp_errno = errno;
                    GT_ERROR_LOG(GT_MOD_CORE, "Read error! errno:%d, %s", tmp_errno, strerror(tmp_errno));
                    goto out;
                }
                else if ((EAGAIN == ret) && (EWOULDBLOCK == ret))
                {
                    continue;
                }
                else
                {
                    //pos += ret;
                    //if (pos >= 15)
                    {
                        buf[ret] = '\0';
                        GT_INFO_LOG(GT_MOD_CORE, "%s", buf);
                        //pos = 0;
                    }
                }
            }
        }
    }
out:
    epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, &ev);
    close(epfd);
    close(cfd);
    close(lfd);
    return 0;
}
#endif