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

#define UT_SPINLOCK_LOCK_PID_PERF(p_spl) \
do { \
    struct timeval start_tv, end_tv; \
    gettimeofday(&start_tv, (struct timezone *)NULL); \
    GT_SPINLOCK_LOCK_WITH_PID(p_spl); \
    gettimeofday(&end_tv, (struct timezone *)NULL); \
    g_spinlock_cast += ((end_tv.tv_sec * 1000000 + end_tv.tv_usec) - (start_tv.tv_sec * 1000000 + start_tv.tv_usec)); \
} while (0)

#define UT_SPINLOCK_UNLOCK_PID_PERF(p_spl) \
do { \
    GT_SPINLOCK_UNLOCK_WITH_PID(p_spl); \
} while (0)

__thread pid_t gt_tid;

void *pthread_routine_cas_origin(void *args)
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

void *pthread_routine_cas_pid(void *args)
{
    gt_spinlock_t *lock = (gt_spinlock_t *)args;
    for (int i = 0; i < 10000; i++)
    {
        UT_SPINLOCK_LOCK_PID_PERF(lock);
        g_total_count++;
        UT_SPINLOCK_UNLOCK_PID_PERF(lock);
    }
    return 0;
}

int pthread_startup(pthread_t *thread_id, int thread_num, gt_spinlock_t *lock, void *(* routine)(void *pthread_attr_getscope))
{
    int idx = 0;
    for (idx = 0; idx < thread_num; idx++)
    {
        if (pthread_create(&thread_id[idx], NULL, routine, lock) != GT_OK)
        {
            GT_ERROR_LOG(GT_MOD_TEST, "Failed to create pthead, idx[%d]", idx);
            return idx;
        }
        GT_INFO_LOG(GT_MOD_TEST, "Success to create pthread[%lu]", thread_id[idx]);
    }
    return idx;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s [thead_num] [lock_type]\n"
                "\tthread_num: number of pthread\n"
                "\tlock_type:\n"
                "\t\t1 -- cas val with 0 and 1\n"
                "\t\t2 -- cas val with pid\n"
                "\t\t3 -- asm val with 0 and 1\n"
                "\t\t4 -- asm val with pid\n"
                , argv[0]);
        return GT_ERROR;
    }
    int idx = 0;
    int thread_num = atoi(argv[1]);
    int lock_type = atoi(argv[2]);
    pthread_t *thread_id = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    memset(thread_id, 0x0, (sizeof(pthread_t) * thread_num));
    
    gt_spinlock_t lock;
    UT_SPINLOCK_INIT(&lock);

    switch (lock_type) {
        case 1: {
            idx = pthread_startup(thread_id, thread_num, &lock, pthread_routine_cas_origin);
            break;
        }
        case 2: {
            idx = pthread_startup(thread_id, thread_num, &lock, pthread_routine_cas_pid);
            break;
        }
        case 3: {}
        case 4: {}
        default: {}
    }

    while (idx--) {
        pthread_join(thread_id[idx], NULL);
    }
    GT_INFO_LOG(GT_MOD_TEST, "spinlock ave cast:%luus, thread_num:%d, total_count:%u",
        g_spinlock_cast / thread_num, thread_num, g_total_count);
    free(thread_id);
    return 0;
}
