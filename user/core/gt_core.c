/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_core.c
 * Module   :core Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the core.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
/* USER INCLUDE */
#include "public.h"
#include "gt_pub.h"
#include "gt_log.h"
#include "gt_core.h"
#include "gt_memory.h"
#include "gt_poller.h"
#include "gt_spinlock.h"

/* STDLIBC INCLUDE */

struct gt_poller g_gt_core_poller;

gt_phase_info_t g_gt_phase[] = {
    [GT_PHASE_0] = {GT_PHASE_0, "GT_PHASE_0", {NULL}},
    [GT_PHASE_1] = {GT_PHASE_1, "GT_PHASE_1", {NULL}},
    [GT_PHASE_2] = {GT_PHASE_2, "GT_PHASE_2", {NULL}},
    [GT_PHASE_3] = {GT_PHASE_3, "GT_PHASE_3", {NULL}},
};

int32_t gt_phase_register(gt_cmod_e cmod, gt_phase_e gphase, phase_fn fn)
{
    //int32_t ret = GT_OK;
    if (g_gt_phase[gphase].fn[cmod])
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Warning: Phase_fn of the cmod[%d] at phase[%d] has been registered!", cmod, gphase);
        return GT_ERROR;
    }
    g_gt_phase[gphase].fn[cmod] = fn;
    return GT_OK;
}

int32_t gt_core_phase0_func(void)
{
    int32_t ret = GT_OK;
    ret = gt_poller_init(GT_CORE_MAXEVENT, GT_CORE_EPWAIT_TIMEOUT, &g_gt_core_poller);
    if (GT_OK != ret)
    {
        //GT_ERROR_LOG(GT_MOD_CORE, "Failed to init poller.");
        return ret;
    }
    return GT_OK;
}

int32_t gt_core_init(void)
{
    int32_t ret = GT_OK;
    ret = gt_phase_register(GT_CMOD_CORE, GT_PHASE_0, gt_core_phase0_func);
    if (GT_OK != ret)
    {
        //GT_ERROR_LOG(GT_MOD_CORE, "Failed to register phase.");
        return ret;
    }

    return GT_OK;
}

int32_t gt_phase_run(void)
{
    int32_t ret = GT_OK;
    int32_t cmod = 0;
    int32_t phase = 0;
    for (phase = 0; phase < DIM(g_gt_phase); phase++)
    {
        for (cmod = 0; cmod < GT_CMOD_MAX; cmod++)
        {
            if (g_gt_phase[phase].fn[cmod])
            {
                GT_INFO_LOG(GT_MOD_CORE, "Sys-init phase[%d] cmod[%d]", phase, cmod);
                ret = g_gt_phase[phase].fn[cmod]();
                if (GT_OK != ret)
                {
                    GT_ERROR_LOG(GT_MOD_CORE, "Failed to run phase_fn on cmod[%d] at %s.", cmod, g_gt_phase[phase].phase);
                    return ret;
                }
            }
        }
    }
    return GT_OK;
}
