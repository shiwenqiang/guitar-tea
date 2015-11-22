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
#include "gt_pub.h"
#include "gt_log.h"


char g_log_level_str[GT_LOG_LEVEL_MAX] = {
    "INFO",
    "WARN",
    "ERROR",
    "DEBUG",
    "UNKOWN"
};

void gt_log_print(gt_mid_e mid, gt_int32_t fd, gt_int32_t log_level, const char *format, ...)
{
    char buf[GT_LOG_BUF_MAX_LEN] = {0};
    struct timeval tv = {0};
    struct tm tm = {0};
    
    /* Fill buffer */
    if (-1 == gettimeofday(&tv, (struct timezone *)NULL))
    {
        return;
    }
    
    if (NULL == localtime_r(&tv.tv_sec, &tm))
    {
        return;
    }

    gt_int32_t ret_time_len = strftime(buf, GT_LOG_BUF_MAX_LEN - 1, "%Y %T", &tm);
    if (ret_time_len < 0)
    {
        return;
    }


    gt_int32_t ret_level_len = snprintf(buf + ret_time_len, GT_LOG_BUF_MAX_LEN - ret_time_len - 1, "[%s]", g_log_level_str[log_level]);
    if (ret_level_len < 0)
    {
        return;
    }

    va_list args;
    
    va_start(args, format);
    gt_int32_t ret_log_len = vsnprintf(buf + ret_time_len + ret_level_len, GT_LOG_BUF_MAX_LEN - ret_time_len - ret_level_len - 1, format, args);
    if (ret_log_len < 0)
    {
        return;
    }
    va_end(args);

    gt_int32_t log_length;
    
    if (GT_LOG_BUF_MAX_LEN - ret_time_len - ret_level_len - 1 > ret_log_len)
    {
        log_length = ret_time_len + ret_level_len + ret_log_len;
    }
    else
    {
        log_length = ret_time_len + ret_level_len + ret_log_len - 1;
    }

    gt_int32_t ret_end_len = snprintf(buf + log_length, GT_LOG_BUF_MAX_LEN - log_length - 1, "/n");
    if (ret_end_len < 0)
    {
        return;
    }
    
    ret_written_len = write(fd, buf, GT_LOG_BUF_MAX_LEN);
    if (ret_written_len < 0)
    {
        return;
    }
    
    return;
    
}
