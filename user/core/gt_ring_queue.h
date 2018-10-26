/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_ring_queue.h
 * Module   :ring_queue
 * Date     :2018/06/26
 * Author   :shiwenqiang
 * Brief    :Declare the data structure: ring queue
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
  **************************************************************/

#ifndef __GT_RING_QUEUE_H__
#define __GT_RING_QUEUE_H__

#define ADDR_BIT_LEN (40)

//ring queue type
typedef enum rq_type {
    GT_RQ_TYPE_SPSC,
    GT_RQ_TYPE_SPMC,
    GT_RQ_TYPE_MPSC,
    GT_RQ_TYPE_MPMC,
    
    GT_RQ_TYPE_MAX
}ring_queue_type_e;

typedef struct gt_ring_data {
    union {
        struct {
            uint64_t val:ADDR_BIT_LEN;
            uint64_t ver:(64 - ADDR_BIT_LEN);
        }data;

        uint64_t uint64_addr;
    };
}gt_ring_data_t;

#define CACHE_LINE_SIZE (64)
typedef struct gt_ring_queue {
    
    struct {
        volatile uint32_t head;
        volatile uint32_t tail;
    }prod;
    
    struct {
        volatile uint32_t head;
        volatile uint32_t tail;
    }cons;
    
    uint32_t size;
    gt_ring_data_t data[0];
}gt_ring_queue_t;

//multi product
int gt_ring_queue_mp_enqueue(gt_ring_queue_t *rq, void *val);

//multi consume
void *gt_ring_queue_mc_dequeue(gt_ring_queue_t *rq);

//single product
int gt_ring_queue_sp_enqueue(gt_ring_queue_t *rq, void *val);

//single consume
void *gt_ring_queue_sc_dequeue(gt_ring_queue_t *rq);




#endif //__GT_RING_QUEUE_H__
