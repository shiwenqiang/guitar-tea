/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_memory.c
 * Module   :Memory Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the memory.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
/* USER INCLUDE */
#include "gt_pub.h"
#include "gt_memory.h"

/* STDLIBC INCLUDE */
#include <malloc.h>

void *gt_malloc_mem(gt_mod_type_e mod, gt_mem_type_e mtype, int64_t size)
{
    return malloc(size);
}

void gt_free_mem(void *mem)
{
    free(mem);
}
