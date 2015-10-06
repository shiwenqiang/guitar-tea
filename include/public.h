#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#define MAX_EVENTS (10)

/* Define Struct */

typedef struct poller{
    int epfd;
    int events;
    int max_events;
}poller_t;

typedef struct net_conn{
    int sfd;
    int cnt;
    struct net_conn *next;
}net_conn_t;

#define GT_TRUE (1)
#define GT_FALSE (0)

#else
#endif
