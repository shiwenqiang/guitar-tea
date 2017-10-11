/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_worker.h
 * Module   :Worker Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the Worker.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_WORKER_H__
#define __GT_WORKER_H__

/* Define Macro */
#define GT_PTHREAD_NUM (5)
#define _GNU_SOURCE
/* Define Structure */
typedef void *(*routine_fn)(void *arg);
typedef struct gt_pthread
{
    pthread_t pthread_id;
    char *pthread_name;
    routine_fn pthread_routine;
    pid_t pid;
}gt_pthread_t;

/* Declare Function Prototype */
extern int32_t gt_create_pthread(void);
extern int32_t gt_wakeup_pthread(void);

extern pid_t gt_get_pthread_pid(void);
extern pthread_t gt_get_pthread_id(void);
#endif /* __GT_WORKER_H__ */
