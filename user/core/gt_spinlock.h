/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_spinlock.h
 * Module   :Multithread Sync Engine
 * Date     :2018/10/28
 * Author   :shiwenqiang
 * Brief    :Implementation the spinlock.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2018-10-28  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_SPINLOCK_H__
#define __GT_SPINLOCK_H__

typedef struct gt_spinlock {
    volatile int locked;
}gt_spinlock_t;

#ifdef __STATIC_INLINE__
#define STATIC static
#define INLINE inline
#else
#define STATIC
#define INLINE
#endif //__STATIC_INLINE__

#ifdef __SSE2__
#include <xmmintrin.h>
STATIC INLINE
#define GT_PAUSE() \
do { \
    _mm_pause(); \
}while (0)
#else
STATIC INLINE
#define GT_PAUSE()
#endif

STATIC INLINE
void gt_spinlock_init(gt_spinlock_t *spl)
{
    spl->locked = 0;
}

STATIC INLINE
void gt_spinlock_lock(gt_spinlock_t *spl)
{
    while(!__sync_bool_compare_and_swap(&spl->locked, 0, 1))
    {
        GT_PAUSE();
    }
}

STATIC INLINE
void gt_spinlock_unlock(gt_spinlock_t *spl)
{
    //spl->locked = 0;
    __sync_lock_release(&spl->locked);
}

STATIC INLINE
bool gt_spinlock_trylock(gt_spinlock_t *spl)
{
    return __sync_bool_compare_and_swap(&spl->locked, 0, 1);
}

#endif