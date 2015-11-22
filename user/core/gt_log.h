/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_log.h
 * Module   :Log Engine
 * Date     :2015/11/21
 * Author   :shiwenqiang
 * Brief    :Implementation the log.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.   
 *  1   V0.01   2015-11-22  shiwenqiang   Original Version.
 **************************************************************/

#ifndef __GT_LOG_H__
#define __GT_LOG_H__

/* Debug Section */

/* Include Files Section */

/* Macro Define Section */
#define GT_STD_INPUT            (0)
#define GT_STD_OUTPUT           (1)
#define GT_STD_ERROR            (2)

#define GT_LOG_LEVEL_INFO       (0)
#define GT_LOG_LEVEL_WARN       (1)
#define GT_LOG_LEVEL_ERROR      (2)
#define GT_LOG_LEVEL_DEBUG      (3)

#define GT_STD_INFO_LOG(fmt, ...)\
    gt_log_print(GT_STD_OUTPUT, GT_LOG_LEVEL_INFO, "%s:%d", fmt, __FUNCTION__, __LINE__, ...);
#define GT_STD_WARN_LOG(fmt, ...)\
    gt_log_print(GT_STD_OUTPUT, GT_LOG_LEVEL_WARN, "%s:%d", fmt, __FUNCTION__, __LINE__, ...);
#define GT_STD_ERROR_LOG(fmt, ...)\
    gt_log_print(GT_STD_OUTPUT, GT_LOG_LEVEL_ERROR, "%s:%d", fmt, __FUNCTION__, __LINE__, ...);
#define GT_STD_DEBUG_LOG(fmt, ...)\
    gt_log_print(GT_STD_OUTPUT, GT_LOG_LEVEL_DEBUG, "%s:%d", fmt, __FUNCTION__, __LINE__, ...);

#define GT_

/* Structure Define Section */

/* Prototype Declare Section */
void gt_log_print(gt_int32_t fd, gt_int32_t level, const char *format, ...);


#endif /*__GT_LOG_H__*/
