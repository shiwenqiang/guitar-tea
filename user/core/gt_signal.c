/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_signal.c
 * Module   :Signal Engine
 * Date     :2015/11/21
 * Author   :shiwenqiang
 * Brief    :Implementation the signal.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-12  shiwenqiang   Original Version.
 **************************************************************/
/* USER INCLUDE */
#include "public.h"
#include "gt_pub.h"
#include "gt_log.h"
#include "gt_signal.h"

/* STDLIBC INCLUDE */

gt_sig_e g_gt_sig_ignore[] = {
    GT_SIGINT,
    GT_SIGIOT,
    GT_SIGPIPE,
    GT_SIGALRM,
    GT_SIGSTKFLT,
    GT_SIGCLD,
    GT_SIGCHLD,
    GT_SIGCONT,
    GT_SIGTSTP,
    GT_SIGTTIN,
    GT_SIGTTOU,
    GT_SIGXCPU,
    GT_SIGXFSZ,
    GT_SIGVTALRM,
    GT_SIGPROF,
    GT_SIGWINCH,
    GT_SIGPOLL,
    GT_SIGIO,
    GT_SIGPWR,
    GT_SIGUNUSED,
};

gt_sig_e g_gt_sig_core[] = {
    GT_SIGILL,
    GT_SIGABRT,
    GT_SIGBUS,
    GT_SIGFPE,
    GT_SIGSEGV,
    GT_SIGSYS,
};

gt_sig_e g_gt_sig_user[] = {
    GT_SIGHUP,
    GT_SIGQUIT,
    GT_SIGUSR1,
    GT_SIGUSR2,
};

int32_t g_signal_pipefd[2];

void gt_process_dump (gt_sig_e sig)
{
    GT_INFO_LOG(GT_MOD_CORE, "Notice: Dump process now!");
    return;
}

void gt_process_snap (gt_sig_e sig)
{
    GT_INFO_LOG(GT_MOD_CORE, "Notice: Snap process now!");
    return;
}

void gt_process_reconf (gt_sig_e sig)
{
    GT_INFO_LOG(GT_MOD_CORE, "Notice: Reconf process now!");
    return;
}

int32_t gt_set_pthread_sigmask(gt_sig_e sig)
{
    int32_t ret;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    ret = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set pthread sigmask! errno:%d, %s", errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}

int32_t gt_create_signal_pipefd(void)
{
    int32_t ret = GT_OK;
    ret = gt_create_pipefd(g_signal_pipefd);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create signal pipefd!");
        return ret;
    }

    /* set write-end to nonblock */
    ret = gt_set_nonblock(g_signal_pipefd[1]);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set signal pipefd to nonblock!");
        close(g_signal_pipefd[0]);
        close(g_signal_pipefd[1]);
        return ret;
    }

    /* set write-end to close-on-exec */
    ret = gt_set_cloexec(g_signal_pipefd[1]);
    if (GT_OK != ret)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set signal pipefd to close-on-exec!");
        close(g_signal_pipefd[0]);
        close(g_signal_pipefd[1]);
        return ret;
    }

    /* Add to Unified-event-source of main-framework */

    return GT_OK;
}

void gt_signal_deliver(int32_t sig)
{
    int32_t msg = sig;
    int32_t save_errno = errno;
    int32_t ret = write(g_signal_pipefd[1], &msg, sizeof(msg));
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to write signal to pipefd[%d]! errno:%d, %s", g_signal_pipefd[1], errnum, strerror(errnum));
    }
    errno = save_errno;
}

int32_t gt_add_signal_handler(int32_t sig, sa_handler_fn sig_handler)
{
    int32_t ret = GT_OK;

    struct sigaction sa;
    memset(&sa, 0x0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigfillset(&sa.sa_mask);
    sa.sa_flags |= SA_RESTART;

    ret = sigaction(sig, &sa, NULL);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to add signal handler! errno:%d, %s", errnum, strerror(errnum));
        return ret;
    }
    return GT_OK;
}

int32_t gt_signal_handle_user(void)
{
    int32_t ret = GT_OK;
    for (int32_t i = 0; i < DIM(g_gt_sig_user); i++)
    {
        GT_INFO_LOG(GT_MOD_CORE, "Set handle to user sig:%d", g_gt_sig_user[i]);
        ret = gt_add_signal_handler(g_gt_sig_user[i], gt_signal_deliver);
        if (GT_OK != ret)
        {
            GT_ERROR_LOG(GT_MOD_CORE, "Failed to register user signal handler!");
            return ret;
        }
    }
    return GT_OK;
}

int32_t gt_signal_handle_core(void)
{
    int32_t ret = GT_OK;
    for (int32_t i = 0; i < DIM(g_gt_sig_core); i++)
    {
        GT_INFO_LOG(GT_MOD_CORE, "Set handle to user sig:%d", g_gt_sig_core[i]);
        ret = gt_add_signal_handler(g_gt_sig_core[i], gt_signal_deliver);
        if (GT_OK != ret)
        {
            GT_ERROR_LOG(GT_MOD_CORE, "Failed to register core signal handler!");
            return ret;
        }
    }
    return GT_OK;
}

int32_t gt_signal_handle_ignore(void)
{
    int32_t ret;
    sigset_t igr_sig;
    sigemptyset(&igr_sig);
    for (int32_t i = 0; i < DIM(g_gt_sig_ignore); i++)
    {
        ret = sigaddset(&igr_sig, g_gt_sig_ignore[i]);
        if (GT_OK != ret)
        {
            int32_t errnum = errno;
            GT_ERROR_LOG(GT_MOD_CORE, "Failed to add igr_sig to set! errno:%d, %s", errnum, strerror(errnum));
            return ret;
        }
    }

    ret = sigprocmask(SIG_BLOCK, &igr_sig, NULL);
    if (GT_OK != ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to set sigprocmask ! errno:%d, %s", errnum, strerror(errnum));
        return ret;
    }

    return GT_OK;
}

int32_t gt_signals_register(void)
{
    /* create signal piepfd */
    gt_create_signal_pipefd();
    /* register user signals */
    gt_signal_handle_user();
    /* handle core signals */
    gt_signal_handle_core();
    /* handle Ignore signals */
    gt_signal_handle_ignore();
    /* handle Default signals */
    // Nothing to do
    return GT_OK;
}
