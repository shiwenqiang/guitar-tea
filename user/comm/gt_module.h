/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_log.c
 * Module   :Log Engine
 * Date     :2015/11/22
 * Author   :shiwenqiang
 * Brief    :Implementation the log.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2015-11-22  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_MODULE_H__
#define __GT_MODULE_H__

typedef enum gt_mod_type
{
    GT_MOD_CORE,
    GT_MOD_MEM,
    GT_MOD_NET,
    GT_MOD_TEST,
    GT_MOD_MAX,
}gt_mod_type_e;

#endif /* __GT_MODULE_H__ */
