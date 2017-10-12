/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_memory.h
 * Module   :Memory Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the memory.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_MEMORY_H__
#define __GT_MEMORY_H__

typedef enum gt_mem_type
{
    GT_MEM_TYPE_HEAP,
    GT_MEM_TYPE_FML,
    GT_MEM_TYPE_BLK,
    GT_MEM_TYPE_INVALID,
}gt_mem_type_e;

extern void *gt_malloc_mem(gt_mod_type_e mod, gt_mem_type_e mtype, int64_t size);
extern int32_t gt_free_mem(gt_mod_type_e mod, gt_mem_type_e mtype, void *mem);

#endif /* __GT_MEMORY_H__ */
