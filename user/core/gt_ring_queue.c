/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_ring_queue.c
 * Module   :ring_queue
 * Date     :2018/06/26
 * Author   :shiwenqiang
 * Brief    :Implementation the interface of ring queue
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 **************************************************************/
/* USER INCLUDE */
#include "gt_pub.h"
#include "gt_log.h"
#include "gt_ring_queue.h"

/* SYSTEM INCLUDE */


//multi product
int gt_ring_queue_mp_enqueue(gt_ring_queue_t *rq, void *val)
{
    return GT_OK;
}

//multi consume
void *gt_ring_queue_mc_dequeue(gt_ring_queue_t *rq);

//single product
int gt_ring_queue_sp_enqueue(gt_ring_queue_t *rq, void *val);

//single consume
void *gt_ring_queue_sc_dequeue(gt_ring_queue_t *rq);

