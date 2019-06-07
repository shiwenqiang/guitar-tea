/**************************************************************
 *          Copyright 2019. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_comm.h
 * Module   :gt common header
 * Date     :2019/06/07
 * Author   :shiwenqiang
 * Brief    :define common component.
 *
 *------------------------Revision History---------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2019-06-07  shiwenqiang   Basic Data Type Orig-
 *                                         inal Version.
 **************************************************************/
#ifndef __GT_COMM_H__
#define __GT_COMM_H__

#ifdef __STATIC_INLINE__
#define STATIC static
#define INLINE inline
#else
#define STATIC
#define INLINE
#endif //__STATIC_INLINE__


/* declare common function */
pid_t gt_gettid();

#endif //__GT_COMM_H__
