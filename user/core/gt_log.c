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


int32_t g_gt_log_fd = GT_STD_OUTPUT_FD;

char *g_log_level_str[GT_LOG_LEVEL_MAX] = {
    "INFO",
    "WARN",
    "ERROR",
    "DEBUG",
};


void gt_log_print(gt_mod_type_e mod, int32_t stdfd_flag, int32_t log_level, const char *format, ...)
{
    int32_t fd;
    char buf[GT_LOG_BUF_MAX_LEN] = {0};
    struct timeval tv;
    struct tm tm;

    /* Fill buffer */
    if (-1 == gettimeofday(&tv, (struct timezone *)NULL))
    {
        return;
    }

    localtime_r(&(tv.tv_sec), &tm);

    int32_t ret_time_len = strftime(buf, GT_LOG_BUF_MAX_LEN - 1, "%F %T ", &tm);
    if (ret_time_len < 0)
    {
        return;
    }

    int32_t ret_level_len = snprintf(buf + ret_time_len, GT_LOG_BUF_MAX_LEN - ret_time_len - 1, "[%s] ", g_log_level_str[log_level]);
    if (ret_level_len < 0)
    {
        return;
    }

    va_list args;

    va_start(args, format);
    int32_t ret_log_len = vsnprintf(buf + ret_time_len + ret_level_len, GT_LOG_BUF_MAX_LEN - ret_time_len - ret_level_len - 1, format, args);
    if (ret_log_len < 0)
    {
        return;
    }
    va_end(args);

    int32_t log_length;

    if (GT_LOG_BUF_MAX_LEN - ret_time_len - ret_level_len - 1 > ret_log_len)
    {
        log_length = ret_time_len + ret_level_len + ret_log_len;
    }
    else
    {
        log_length = ret_time_len + ret_level_len + ret_log_len - 1;
    }

    int32_t ret_end_len = snprintf(buf + log_length, GT_LOG_BUF_MAX_LEN - log_length - 1, "\n");
    if (ret_end_len < 0)
    {
        return;
    }

    if (stdfd_flag)
    {
        fd = GT_STD_OUTPUT_FD;
    }
    else
    {
        fd = g_gt_log_fd;
    }

    int32_t ret_written_len = write(fd, buf, log_length + ret_end_len);
    if (ret_written_len < 0)
    {
        return;
    }

    //sync();
    return;
}

int32_t gt_log_init(void)
{
    g_gt_log_fd = open(GT_LOG_FILE_PATH""GT_LOG_FILE_NAME, O_CREAT|O_APPEND|O_RDWR, 0666);
    if (0 > g_gt_log_fd)
    {
        int32_t errnum = errno;
        GT_STD_ERROR_LOG(GT_MOD_CORE, "Failed to open log-file: %s%s! errno:%d, %s", GT_LOG_FILE_PATH, GT_LOG_FILE_NAME, errnum, strerror(errnum));
        return GT_ERROR;
    }

    return GT_OK;
}

