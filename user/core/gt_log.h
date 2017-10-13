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

#define GT_STD_INPUT_FD            (0)
#define GT_STD_OUTPUT_FD           (1)
#define GT_STD_ERROR_FD            (2)

#define GT_LOG_BUF_MAX_LEN      (1024)
#define GT_LOG_FILE_PATH        "/tmp/"
#define GT_LOG_FILE_NAME        "gt_server.log"

/* Structure Define Section */
typedef enum gt_log_level
{
    GT_LOG_LEVEL_INFO,
    GT_LOG_LEVEL_WARN,
    GT_LOG_LEVEL_ERROR,
    GT_LOG_LEVEL_DEBUG,

    GT_LOG_LEVEL_MAX,
}gt_log_level_e;

/* Prototype Declare Section */
void gt_log_print(gt_mod_type_e mod, int32_t stdfd_flag, int32_t level, const char *format, ...)__attribute__((format(printf, 4, 5)));
extern int32_t g_gt_log_fd;


#define GT_STD_INFO_LOG(mod, fmt, arg...) \
    gt_log_print(mod, GT_TRUE, GT_LOG_LEVEL_INFO, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);
#define GT_STD_WARN_LOG(mod, fmt, arg...) \
    gt_log_print(mod, GT_TRUE, GT_LOG_LEVEL_WARN, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);
#define GT_STD_ERROR_LOG(mod, fmt, arg...)\
    gt_log_print(mod, GT_TRUE, GT_LOG_LEVEL_ERROR, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);
#define GT_STD_DEBUG_LOG(mod, fmt, arg...)\
    gt_log_print(mod, GT_TRUE, GT_LOG_LEVEL_DEBUG, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);

#define GT_INFO_LOG(mod, fmt, arg...) \
    gt_log_print(mod, GT_FALSE, GT_LOG_LEVEL_INFO, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);
#define GT_WARN_LOG(mod, fmt, arg...) \
    gt_log_print(mod, GT_FALSE, GT_LOG_LEVEL_WARN, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);
#define GT_ERROR_LOG(mod, fmt, arg...)\
    gt_log_print(mod, GT_FALSE, GT_LOG_LEVEL_ERROR, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);
#define GT_DEBUG_LOG(mod, fmt, arg...)\
    gt_log_print(mod, GT_FALSE, GT_LOG_LEVEL_DEBUG, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg);


extern int32_t gt_log_init(void);


#endif /*__GT_LOG_H__*/
