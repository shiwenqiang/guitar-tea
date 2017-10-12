/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :main.c
 * Module   :Framework Engine
 * Date     :2017/10/11
 * Author   :shiwenqiang
 * Brief    :Implementation the main framework.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-11  shiwenqiang   Original Version.
 **************************************************************/
/* USER INCLUDE */
#include "public.h"
#include "gt_pub.h"
#include "gt_log.h"
#include "gt_signal.h"
#include "gt_worker.h"
#include "gt_memory.h"

/* STDLIBC INCLUDE */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

extern uint32_t g_gt_server_ip;
extern uint16_t g_gt_server_port;

int32_t gt_parse_parameters(int argc, char *argv[])
{
    if (argc < 3)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Invalid Parameters. Usage: %s [ip] [port]", argv[0]);
        return GT_ERROR;
    }

    struct in_addr addr;
    if (0 == inet_aton(argv[1], &addr))
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Invalid ip[%s]. Usage: %s [ip] [port]", argv[1], argv[0]);
        return GT_ERROR;
    }

    g_gt_server_ip = ntohl(addr.s_addr);
    g_gt_server_port = atoi(argv[2]);
    return GT_OK;
}

int32_t gt_daemonize_process(void)
{
    //int32_t ret = 0;
    //int32_t errnum = 0;

    pid_t pid = 0;

    /* Clear file creation mask */
    umask(0);

    /* First fork */
    pid = fork();
    if (0 < pid)
    {
        _exit(0);
    }
    else if (0 > pid)
    {
        return GT_ERROR;
    }

    setsid();
    /* Second fork */
    pid = fork();
    if (0 < pid)
    {
        _exit(0);
    }
    else if (0 > pid)
    {
        return GT_ERROR;
    }

    chdir("/");

    /* Close STDIN\STDOUT\STDERR */
    close(0);
    close(1);
    close(2);

    int fd0 = open("/dev/null", O_RDWR);
    dup2(fd0, 1);
    dup2(fd0, 2);

    return GT_OK;
}

int32_t gt_init_system(void)
{
    int ret = 0;
    /* Memory */
    /* Logging */
    /* Signal */
    ret = gt_signals_register();
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to register signals!");
        return ret;
    }
    /* pthread */
    ret = gt_create_pthread();
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create pthread!");
        return ret;
    }

    sleep(2);

    /* wakeup-pthread */
    ret = gt_wakeup_pthread();
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to wakeup pthread!");
        return ret;
    }

    return GT_OK;
}

int32_t gt_do_run(void)
{
    pid_t pid = gt_get_pthread_pid();
    pthread_t tid = gt_get_pthread_id();
    prctl(PR_SET_NAME, "gt_pthread_main");
    while(1)
    {
        sleep(1);
        GT_INFO_LOG(GT_MOD_CORE, "This is main, [pthread:%lu, pid:%d]", tid, pid);
    }

    return GT_OK;
}

int32_t main(int argc, char *argv[])
{
    int ret = GT_OK;
    /* Parse Parameters */
    ret = gt_parse_parameters(argc, argv);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to parse paramters!");
        return ret;
    }

    /* Daemonize */
    ret = gt_daemonize_process();
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to daemonize process!");
        return ret;
    }

    /* Init system */
    ret = gt_init_system();
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to init process!");
        return ret;
    }

    /* do-run */
    ret = gt_do_run();
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to do-run!");
        return ret;
    }

    return GT_OK;
}
