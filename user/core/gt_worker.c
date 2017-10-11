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

pid_t gt_get_pthread_pid(void)
{
    return syscall(__NR_gettid);
}

pthread_t gt_get_pthread_id(void)
{
    return pthread_self();
}

void * gt_pthread_routine(void * args)
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
        GT_INFO_LOG(GT_MID_CORE, "This is %s, [thread:%lu, pid:%d]", gt_pthread->pthread_name, gt_pthread->pthread_id, gt_pthread->pid);
    }
}

gt_pthread_t g_gt_pthread[GT_PTHREAD_NUM] = {
    {0, "gt_pthread_0", gt_pthread_routine, 0},
    {0, "gt_pthread_1", gt_pthread_routine, 0},
    {0, "gt_pthread_2", gt_pthread_routine, 0},
    {0, "gt_pthread_3", gt_pthread_routine, 0},
    {0, "gt_pthread_4", gt_pthread_routine, 0}
};

int32_t gt_create_pthread(void)
{
    int32_t ret = 0;

    ret = pthread_mutex_init(&gt_pthread_mutex, NULL);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MID_CORE, "Failed to init mutex!");
        return ret;
    }

    ret = pthread_cond_init(&gt_pthread_cond, NULL);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MID_CORE, "Failed to init cond!");
        return ret;
    }

    for (int i = 0; i < GT_PTHREAD_NUM; i++)
    {
        ret = pthread_create(&g_gt_pthread[i].pthread_id, NULL, g_gt_pthread[i].pthread_routine, &g_gt_pthread[i]);
        if (GT_OK != ret)
        {
            int32_t errnum = errno;
            GT_ERROR_LOG(GT_MID_CORE, "Failed create pthread[%d]. errno:%d, %s", i, errnum, strerror(errnum));
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
        GT_ERROR_LOG(GT_MID_CORE, "Failed wakeup all pthread. errno:%d, %s", errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}
