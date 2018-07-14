/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_core.h
 * Module   :Core Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the core.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_CORE_H__
#define __GT_CORE_H__

typedef enum gt_phase
{
    GT_PHASE_0,
    GT_PHASE_1,
    GT_PHASE_2,
    GT_PHASE_3,
    GT_PHASE_MAX,
}gt_phase_e;

/* Submod of core-module */
typedef enum gt_cmod
{
    GT_CMOD_CORE,
    GT_CMOD_MEM,
    GT_CMOD_SIGNAL,
    GT_CMOD_POLLER,
    GT_CMOD_NETWORK,
    GT_CMOD_PTHREAD,
    GT_CMOD_MAX,
}gt_cmod_e;

typedef int32_t (*phase_fn)(void);
typedef struct gt_phase_info
{
    enum gt_phase gphase;
    char phase[16];
    phase_fn fn[GT_CMOD_MAX];
}gt_phase_info_t;

extern int32_t gt_core_init(void);
extern int32_t gt_phase_run(void);
extern int32_t gt_phase_register(gt_cmod_e cmod, gt_phase_e gphase, phase_fn fn);

#endif /* __GT_CORE_H__ */
