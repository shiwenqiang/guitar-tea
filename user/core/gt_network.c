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
#include "gt_core.h"
#include "gt_poller.h"
#include "gt_worker.h"

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

struct gt_poller g_gt_network_poller;
struct gt_poller g_gt_datanet_poller[GT_DATANET_PTHREAD_NUM];
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


int32_t gt_datanet_ops_read(struct gt_event *ge)
{
    int32_t ret = GT_OK;
    char buf[128] = {0};
    ret = read(ge->efd, buf, sizeof(buf) - 1);
    if (ret <= 0)
    {
        int32_t tmp_errno = errno;
        if (EAGAIN == tmp_errno || EWOULDBLOCK == tmp_errno)
        {
            return GT_OK;
        }
        else {
            GT_INFO_LOG(GT_MOD_CORE, "Failed to read fd:%d, errno:%d", ge->efd, errno);
            gt_poller_event_stop(EPOLLIN, ge);
            gt_poller_gevent_destroy(ge);
        }
        return ret;
    }
    GT_INFO_LOG(GT_MOD_CORE, "CFD[%d]: %s", ge->efd, buf);
    return ret;
}

extern gt_pthread_t g_gt_datanet_pthread[GT_DATANET_PTHREAD_NUM];

int32_t gt_network_round_dispatch(int32_t sockfd)
{
    int32_t pthread_idx = sockfd % DIM(g_gt_datanet_pthread);
    struct gt_poller *gpoller = g_gt_datanet_pthread[pthread_idx].data;
    struct gt_event *ge = gt_poller_gevent_create(sockfd, gpoller);
    if (NULL == ge)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create gt_event for datanet[%d].", pthread_idx);
        return GT_OK;
    }
    ge->ops.read = gt_datanet_ops_read;

    /* start POLLIN for signal pipefd */

    if (GT_OK !=  gt_poller_event_start(EPOLLIN, ge))
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to start poller event! sockfd[%d], pthread_idx[%d]", sockfd, pthread_idx);
        return GT_ERROR;
    }
    return GT_OK;
}

int32_t gt_network_ops_read(struct gt_event* ge)
{
    struct sockaddr_in caddr;
    memset(&caddr, 0x0, sizeof(caddr));
    socklen_t caddrlen = sizeof(caddr);
    int32_t cfd = accept(ge->efd, (struct sockaddr *)&caddr, &caddrlen);
    if (-1 == cfd)
    {
        int32_t tmp_errno = errno;
        if (EAGAIN == tmp_errno || EWOULDBLOCK == tmp_errno)
        {
            return GT_OK;
        }
        else
        {
            GT_ERROR_LOG(GT_MOD_CORE, "Failed to accept new conn on lfd[%d]! errno:%d, %s", ge->efd, tmp_errno, strerror(tmp_errno));
            return GT_ERROR;
        }
    }

    int32_t opt = 0;
    if (-1 == setsockopt(cfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set socketopt! errno:%d, %s", tmp_errno, strerror(tmp_errno));
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
        close(cfd);
        cfd = -1;
        return -1;
    }

    if (GT_OK != gt_network_round_dispatch(cfd))
    {
        return GT_ERROR;
    }

    GT_INFO_LOG(GT_MOD_CORE, "Connection form IP[%#x], PORT[%d]", ntohl(caddr.sin_addr.s_addr), ntohs(caddr.sin_port));
    return GT_OK;
}

int32_t gt_network_listenning(int32_t ip, int32_t port)
{

    /* init local variables */
    int lfd = -1;
    struct sockaddr_in saddr;
    memset(&saddr, 0x0, sizeof(saddr));

    /* create listen and accept */
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 > lfd)
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create socket! errno:%d, %s", tmp_errno, strerror(tmp_errno));
        return GT_ERROR;
    }
    int opt = 0;
    if (-1 == setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set socketopt for lfd! errno:%d, %s", tmp_errno, strerror(tmp_errno));
        close(lfd);
        lfd = -1;
        return GT_ERROR;
    }
    
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(ip);
    if (-1 == bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr)))
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to bind ip and port to lfd[%d]! errno:%d, %s", lfd, tmp_errno, strerror(tmp_errno));
        close(lfd);
        return GT_ERROR;
    }

    if (-1 == listen(lfd, GT_NETWORKER_MAXEVENT))
    {
        int32_t tmp_errno = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to listen ip and port on lfd[%d]! errno:%d, %s", lfd, tmp_errno, strerror(tmp_errno));
        close(lfd);
        return GT_ERROR;
    }

    struct gt_event *ge = gt_poller_gevent_create(lfd, &g_gt_network_poller);
    if (NULL == ge)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create gt_event for network[%d].", lfd);
        return GT_ERROR;
    }
    ge->ops.read = gt_network_ops_read;

    /* start POLLIN for signal pipefd */
    if (GT_OK !=  gt_poller_event_start(EPOLLIN, ge))
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to start poller event to lfd[%d]", lfd);
        return GT_ERROR;
    }
    GT_INFO_LOG(GT_MOD_CORE, "Start listenning...");
    return GT_OK;
}

int32_t gt_network_phase1_func(void)
{
    int32_t ret = GT_OK;
    ret = gt_poller_init(GT_NETWORKER_MAXEVENT, GT_NETWORKER_EPWAIT_TIMEOUT, &g_gt_network_poller);
    if (GT_OK != ret)
    {
        //GT_ERROR_LOG(GT_MOD_CORE, "Failed to init poller.");
        return ret;
    }
    ret = gt_network_listenning(g_gt_server_ip, g_gt_server_port);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to listenning, ip:%#x, port:%d", g_gt_server_ip, g_gt_server_port);
        return ret;
    }
    return GT_OK;
}

int32_t gt_network_phase2_func(void)
{
    int32_t ret = GT_OK;
    ret = gt_create_datanet_pthread();
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create datanet pthread.");
        return ret;
    }
    return GT_OK;
}

int32_t gt_network_init(void)
{
    int32_t ret = GT_OK;
    ret = gt_phase_register(GT_CMOD_NETWORK, GT_PHASE_1, gt_network_phase1_func);
    if (GT_OK != ret)
    {
        //GT_ERROR_LOG(GT_MOD_CORE, "Failed to register phase.");
        return ret;
    }

    ret = gt_phase_register(GT_CMOD_NETWORK, GT_PHASE_2, gt_network_phase2_func);
    if (GT_OK != ret)
    {
        //GT_ERROR_LOG(GT_MOD_CORE, "Failed to register phase.");
        return ret;
    }
    return GT_OK;
}
