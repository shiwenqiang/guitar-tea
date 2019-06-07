/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_worker.c
 * Module   :Worker Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the Worker.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
/* USER INCLUDE */
#include "gt_pub.h"
#include "gt_log.h"
#include "gt_poller.h"
#include "gt_worker.h"

/* STDLIBC INCLUDE */
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

pthread_mutex_t gt_pthread_mutex;
pthread_cond_t gt_pthread_cond;

pid_t gt_gettid(void)
{
    static __thread pid_t cached_tid;
    static __thread pid_t cached_pid;
    pid_t pid = getpid();
    if ((cached_pid != pid) || (cached_tid == 0))
    {
        cached_pid = pid;
        cached_tid = syscall(__NR_gettid);
    }
    return cached_tid;
}

pthread_t gt_get_pthread_id(void)
{
    return pthread_self();
}

void * gt_network_pthread_routine(void * args)
{
    //int32_t ret = 0;
    gt_pthread_t *gt_pthread = (gt_pthread_t *)args;
    if (NULL == gt_pthread)
    {
        pthread_exit(NULL);
    }

    struct gt_poller * gpoller = (struct gt_poller *)gt_pthread->data;
    gt_pthread->pid = syscall(__NR_gettid);
    prctl(PR_SET_NAME, gt_pthread->pthread_name);

    pthread_mutex_lock(&gt_pthread_mutex);
    pthread_cond_wait(&gt_pthread_cond, &gt_pthread_mutex);
    pthread_mutex_unlock(&gt_pthread_mutex);

    while (1)
    {
        gt_poller_epoll_wait(gpoller);
    }
}

void * gt_business_pthread_routine(void * args)
{
    //int32_t ret = 0;
    gt_pthread_t *gt_pthread = (gt_pthread_t *)args;
    if (NULL == gt_pthread)
    {
        pthread_exit(NULL);
    }

    gt_pthread->pid = syscall(__NR_gettid);
    prctl(PR_SET_NAME, gt_pthread->pthread_name);

    pthread_mutex_lock(&gt_pthread_mutex);
    pthread_cond_wait(&gt_pthread_cond, &gt_pthread_mutex);
    pthread_mutex_unlock(&gt_pthread_mutex);

    while (1)
    {
        sleep(1);
        //GT_INFO_LOG(GT_MOD_CORE, "This is %s, [thread:%lu, pid:%d]", gt_pthread->pthread_name, gt_pthread->pthread_id, gt_pthread->pid);
    }
}


extern struct gt_poller g_gt_network_poller;
gt_pthread_t g_gt_business_pthread[GT_BUSINESS_PTHREAD_NUM] = {
    {0, "gt_business_0", gt_business_pthread_routine, 0, NULL},
    {0, "gt_business_1", gt_business_pthread_routine, 0, NULL},
    {0, "gt_business_2", gt_business_pthread_routine, 0, NULL},
    {0, "gt_business_3", gt_business_pthread_routine, 0, NULL},
    {0, "gt_network_m", gt_network_pthread_routine, 0, &g_gt_network_poller}
};

int32_t gt_create_pthread(void)
{
    int32_t ret = 0;

    ret = pthread_mutex_init(&gt_pthread_mutex, NULL);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to init mutex!");
        return ret;
    }

    ret = pthread_cond_init(&gt_pthread_cond, NULL);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to init cond!");
        return ret;
    }

    for (int i = 0; i < GT_BUSINESS_PTHREAD_NUM; i++)
    {
        ret = pthread_create(&g_gt_business_pthread[i].pthread_id, NULL, g_gt_business_pthread[i].pthread_routine, &g_gt_business_pthread[i]);
        if (GT_OK != ret)
        {
            int32_t errnum = errno;
            GT_ERROR_LOG(GT_MOD_CORE, "Failed create pthread[%d]. errno:%d, %s", i, errnum, strerror(errnum));
            return GT_ERROR;
        }
    }
    return GT_OK;
}

int32_t gt_wakeup_pthread(void)
{
    int32_t ret = pthread_cond_broadcast(&gt_pthread_cond);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed wakeup all pthread. errno:%d, %s", errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}


void * gt_datanet_pthread_routine(void * args)
{
    //int32_t ret = 0;
    gt_pthread_t *gt_pthread = (gt_pthread_t *)args;
    if (NULL == gt_pthread)
    {
        pthread_exit(NULL);
    }

    gt_pthread->pid = syscall(__NR_gettid);
    prctl(PR_SET_NAME, gt_pthread->pthread_name);

    pthread_mutex_lock(&gt_pthread_mutex);
    pthread_cond_wait(&gt_pthread_cond, &gt_pthread_mutex);
    pthread_mutex_unlock(&gt_pthread_mutex);

    struct gt_poller *gpoller = (struct gt_poller *)gt_pthread->data;
    int32_t ret = gt_poller_init(GT_NETWORKER_MAXEVENT, GT_NETWORKER_EPWAIT_TIMEOUT, gpoller);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to init poller.");
        pthread_exit(NULL);
    }

    while (1)
    {
        gt_poller_epoll_wait(gpoller);
    }
}

extern struct gt_poller g_gt_datanet_poller[GT_DATANET_PTHREAD_NUM];
gt_pthread_t g_gt_datanet_pthread[GT_DATANET_PTHREAD_NUM] = {
    {0, "gt_datanet_0", gt_datanet_pthread_routine, 0, &g_gt_datanet_poller[0]},
    {0, "gt_datanet_1", gt_datanet_pthread_routine, 0, &g_gt_datanet_poller[1]},
    {0, "gt_datanet_2", gt_datanet_pthread_routine, 0, &g_gt_datanet_poller[2]},
    {0, "gt_datanet_3", gt_datanet_pthread_routine, 0, &g_gt_datanet_poller[3]}
};

int32_t gt_create_datanet_pthread(void)
{
    int32_t ret = 0;

    for (int i = 0; i < GT_DATANET_PTHREAD_NUM; i++)
    {
        ret = pthread_create(&g_gt_datanet_pthread[i].pthread_id, NULL, g_gt_datanet_pthread[i].pthread_routine, &g_gt_datanet_pthread[i]);
        if (GT_OK != ret)
        {
            int32_t errnum = errno;
            GT_ERROR_LOG(GT_MOD_CORE, "Failed create pthread[%d]. errno:%d, %s", i, errnum, strerror(errnum));
            return GT_ERROR;
        }
    }
    return GT_OK;
}
