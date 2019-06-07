#include "gt_pub.h"
#include "gt_log.h"
#include "gt_spinlock.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>

unsigned long g_spinlock_cast = 0;
unsigned int g_total_count = 0;
#define UT_SPINLOCK_INIT(p_spl) \
do { \
    GT_SPINLOCK_INIT(p_spl); \
} while (0)

#define UT_SPINLOCK_LOCK_PERF(p_spl) \
do { \
    struct timeval start_tv, end_tv; \
    gettimeofday(&start_tv, (struct timezone *)NULL); \
    GT_SPINLOCK_LOCK(p_spl); \
    gettimeofday(&end_tv, (struct timezone *)NULL); \
    g_spinlock_cast += ((end_tv.tv_sec * 1000000 + end_tv.tv_usec) - (start_tv.tv_sec * 1000000 + start_tv.tv_usec)); \
} while (0)

#define UT_SPINLOCK_UNLOCK_PERF(p_spl) \
do { \
    GT_SPINLOCK_UNLOCK(p_spl); \
} while (0)

void *pthread_routine(void *args)
{
    gt_spinlock_t *lock = (gt_spinlock_t *)args;
    for (int i = 0; i < 10000; i++)
    {
        UT_SPINLOCK_LOCK_PERF(lock);
        g_total_count++;
        UT_SPINLOCK_UNLOCK_PERF(lock);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        GT_ERROR_LOG(GT_MOD_TEST, "Usage: %s [thead_num]", argv[0]);
        return GT_ERROR;
    }
    int idx = 0;
    int thread_num = atoi(argv[1]);
    pthread_t *thread_id = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    memset(thread_id, 0x0, sizeof(pthread_t) * thread_num);
    gt_spinlock_t lock;
    UT_SPINLOCK_INIT(&lock);
    for (idx = 0; idx < thread_num; idx++)
    {
        
        if (pthread_create(&thread_id[idx], NULL, pthread_routine, &lock) != GT_OK)
        {
            GT_ERROR_LOG(GT_MOD_TEST, "Failed to create pthead, idx[%d]", idx);
            _exit(0);
        }
        GT_INFO_LOG(GT_MOD_TEST, "Success to create pthread[%lu]", thread_id[idx]);
    }

    for (idx = 0; idx < thread_num; idx++)
    {
        pthread_join(thread_id[idx], NULL);
    }
    GT_INFO_LOG(GT_MOD_TEST, "spinlock ave cast:%luus, thread_num:%d, total_count:%u",
        g_spinlock_cast / thread_num, thread_num, g_total_count);
    free(thread_id);
    return 0;
}
