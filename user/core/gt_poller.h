/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_poller.h
 * Module   :Poller Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the poller.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_POLLER_H__
#define __GT_POLLER_H__

#include <sys/epoll.h>

#define GT_EPOLL_WAIT_TIMEOUT (50)
#define GT_CORE_MAXEVENT (32)
#define GT_CORE_EPWAIT_TIMEOUT (GT_EPOLL_WAIT_TIMEOUT) //Milliseconds
#define GT_WORKER_MAXEVENT (1024)
#define GT_WORKER_EPWAIT_TIMEOUT (GT_EPOLL_WAIT_TIMEOUT) //Milliseconds

typedef struct gt_poller
{
    int32_t  epfd;
    struct epoll_event *events; //Contain the available events
    int32_t maxe;          //Up to maxevents will be returned
    int32_t timeout;            //Milliseconds

    struct list_head elist;
}gt_poller_t;

typedef struct gt_event
{
    int32_t efd;
    struct epoll_event e;
    struct list_head list;
    struct gt_poller *gpoller;
}gt_event_t;


extern struct gt_event * gt_poller_gevent_create(int32_t efd, struct gt_poller *gpoller);
extern void gt_poller_gevent_destroy(struct gt_event *ge);

extern int32_t gt_poller_event_start(int32_t eflag, struct gt_event *gevent);
extern int32_t gt_poller_event_stop(int32_t eflag, struct gt_event *gevent);

extern int32_t gt_poller_init(int32_t maxe, int32_t timeout, struct gt_poller *gpoller);
extern void gt_poller_clean(struct gt_poller *gpoller);
#endif /* __GT_POLLER_H__ */
