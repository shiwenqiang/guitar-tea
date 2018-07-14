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
#include "gt_memory.h"
#include "gt_poller.h"
/* STDLIBC INCLUDE */


int32_t gt_poller_event_start(int32_t eflag, struct gt_event *gevent)
{
    int32_t ret = GT_OK;
    int32_t op = EPOLL_CTL_ADD;
    struct gt_poller *gpoller = gevent->gpoller;

    if (0 != gevent->e.events)
    {
        op = EPOLL_CTL_MOD;
    }

    if (gevent->e.events & eflag)
    {
        return GT_OK; /* events have been registered */
    }

    gevent->e.events |= eflag;
    gevent->e.data.ptr = gevent;
    ret = epoll_ctl(gpoller->epfd, op, gevent->efd, &gevent->e);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to epoll_ctl to epfd[%d]! errno:%d, %s", gpoller->epfd, errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}

int32_t gt_poller_event_stop(int32_t eflag, struct gt_event *gevent)
{
    int32_t ret = GT_OK;
    int32_t op = EPOLL_CTL_DEL;
    struct gt_poller *gpoller = gevent->gpoller;

    if (0 != gevent->e.events)
    {
        op = EPOLL_CTL_MOD;
    }

    if (!(gevent->e.events & eflag))
    {
        return GT_OK; /* events have been registered */
    }

    gevent->e.events &= ~eflag;
    ret = epoll_ctl(gpoller->epfd, op, gevent->efd, &gevent->e);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to epoll_ctl to epfd[%d]! errno:%d, %s", gpoller->epfd, errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}

struct gt_event * gt_poller_gevent_create(int32_t efd, struct gt_poller *gpoller)
{
    struct gt_event *ge = (struct gt_event *)gt_malloc_mem(GT_MOD_CORE, GT_MEM_TYPE_FML, sizeof(struct gt_event));
    if (NULL == ge)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to alloc mem to gt_event");
        return NULL;
    }

    memset(ge, 0x0, sizeof(struct gt_event));

    ge->efd = efd;
    INIT_LIST_HEAD(&ge->list);
    list_add(&gpoller->elist, &ge->list);
    ge->gpoller = gpoller;

    return ge;
}

void gt_poller_gevent_destroy(struct gt_event *ge)
{
    gt_poller_event_stop(ge->e.events, ge);

    list_del(&ge->list);    //一个poller及其所有gevent只在一个线程下被处理，因此不用加锁

    close(ge->efd);

    gt_free_mem(ge);
}

int32_t gt_poller_init(int32_t maxe, int32_t timeout, struct gt_poller *gpoller)
{
    //int ret = GT_OK;
    gpoller->maxe = maxe;
    gpoller->timeout = timeout;
    gpoller->epfd = epoll_create(maxe + 1);
    if (0 > gpoller->epfd)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to epoll_ctl to epfd[%d]! errno:%d, %s", gpoller->epfd, errnum, strerror(errnum));
        return GT_ERROR;
    }

    gpoller->ee = (struct epoll_event *)gt_malloc_mem(GT_MOD_CORE, GT_MEM_TYPE_HEAP, (maxe * sizeof(struct epoll_event)));
    if (NULL == gpoller->ee)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to alloc mem for gpoller.");
        return GT_ERROR;
    }
    INIT_LIST_HEAD(&gpoller->elist);

    return GT_OK;
}

void gt_poller_clean(struct gt_poller *gpoller)
{
    if (!list_empty(&gpoller->elist))
    {
        GT_WARN_LOG(GT_MOD_CORE, "Warning: List not empty!");
    }

    close(gpoller->epfd);

    gt_free_mem(gpoller->ee);
}

void gt_poller_epoll_wait(struct gt_poller *gpoller)
{
    int32_t ret = GT_OK;
    int events_num = epoll_wait(gpoller->epfd, gpoller->ee,
                                gpoller->maxe, gpoller->timeout);
    if (0 < events_num)
    {
        for (int32_t i = 0; i < events_num; i++)
        {
            struct gt_event *ge = (struct gt_event *)gpoller->ee[i].data.ptr;
            if ( gpoller->ee[i].events & EPOLLIN )
            {
                ret = ge->ops.read(ge);
                if (ret < 0)
                {
                    GT_ERROR_LOG(GT_MOD_CORE, "Failed to read for efd:%d", ge->efd);
                }
            }
            else if( gpoller->ee[i].events & EPOLLOUT )
            {
                ret = ge->ops.write(ge);
            }
        }
    }
}
