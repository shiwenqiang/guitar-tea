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
#include "gt_core.h"
#include "gt_signal.h"
#include "gt_poller.h"

/* STDLIBC INCLUDE */
#define _GNU_SOURCE

#include <dlfcn.h>
#include <execinfo.h>
gt_sig_e g_gt_sig_ignore[] = {
    GT_SIGINT,
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
    GT_SIGIO,
    GT_SIGPWR,
};

gt_sig_e g_gt_sig_core[] = {
    GT_SIGILL,
    GT_SIGABRT,
    GT_SIGBUS,
    GT_SIGFPE,
    GT_SIGSEGV,
    GT_SIGSYS,
};

gt_sig_e g_gt_sig_custom[] = {
    GT_SIGHUP,
    GT_SIGQUIT,
    GT_SIGUSR1,
    GT_SIGUSR2,
};

int32_t g_signal_pipefd[2];

extern struct gt_poller g_gt_core_poller;

void gt_process_dump (gt_sig_e sig)
{
    GT_INFO_LOG(GT_MOD_CORE, "Notice: Dump process now! Signal[%d]", sig);
    return;
}

void gt_process_snap (gt_sig_e sig)
{
    GT_INFO_LOG(GT_MOD_CORE, "Notice: Snap process now! Signal[%d]", sig);
    return;
}

void gt_process_reconf (gt_sig_e sig)
{
    GT_INFO_LOG(GT_MOD_CORE, "Notice: Reconf process now! Signal[%d]", sig);
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

int32_t gt_signal_create_pipefd(void)
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

int32_t gt_strcut(char *dst, const char *src, char cstart, char cend, int32_t flag)
{
    char *begin = strrchr(src, cstart);
    char *end = strrchr(src, cend);
    int32_t sub_len = end - begin + 1;
    if (flag)
    {
        strncpy(dst, begin, sub_len);
    }
    else
    {
        strncpy(dst, begin + 1, sub_len);
    }

    dst[sub_len] = '\0';
    return sub_len;
}

void gt_backtrace_print(int32_t sig)
{
    int32_t nptrs;
    char **strings;
    void *buf[GT_BACKTRACE_BUF_SIZE];
    char frame[GT_BACKTRACE_BUF_SIZE] = {0};

    nptrs = backtrace(buf, GT_BACKTRACE_BUF_SIZE);
    strings = backtrace_symbols(buf, nptrs);
    dprintf(g_gt_log_fd, "Calltrace: Catch Signal %d\n", sig);
    for (int32_t i = 0; i < nptrs; i++)
    {
        int32_t strnum = gt_strcut(frame, strings[i], '[', ']', GT_TRUE);
        gt_strcut(frame + strnum, strings[i], '(', ')', GT_TRUE);
        dprintf(g_gt_log_fd, "%s\n", frame);
    }

    free(strings);
}

void gt_signal_deliver(int32_t sig)
{
    int32_t msg = sig;
    int32_t ret = write(g_signal_pipefd[1], &msg, sizeof(msg));
    if (-1 == ret)
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to write signal to pipefd[%d]! errno:%d, %s", g_signal_pipefd[1], errnum, strerror(errnum));
    }
}

int32_t gt_signal_ops_read(struct gt_event *ge)
{
    int32_t sig;
    int pipefd = ge->efd;
    int32_t ret = read(pipefd, &sig, 1); /* SWQ: Will lost signal??? */
    if (1 == ret)
    {
        switch (sig)
        {
            case GT_SIGHUP:
            case GT_SIGQUIT:
            case GT_SIGUSR1:
            case GT_SIGUSR2:
                {
                    gt_process_snap(sig);
                    break;
                }
            case GT_SIGILL:
            case GT_SIGABRT:
            case GT_SIGBUS:
            case GT_SIGFPE:
            case GT_SIGSEGV:
            case GT_SIGSYS:
                {
                    gt_process_dump(sig);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    else if (0 == ret)
    {
        //Nothing todo
    }
    else //error
    {
        int32_t errnum = errno;
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to get Signal from pipefd, errno:%d, %s", errnum, strerror(errnum));
        return GT_ERROR;
    }
    return GT_OK;

}

#if 0
int callback(struct dl_phdr_info *info, size_t size, void *data)
{
    int32_t j;
    GT_INFO_LOG(GT_MOD_CORE, "name=%s :%d segments", info->dlpi_name, info->dlpi_phnum);
    GT_INFO_LOG(GT_MOD_CORE, "obj base: %10p", (void *)info->dlpi_addr);

    for (j = 0; j < info->dlpi_phnum; j++)   {
        void* addr = (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr);
        GT_INFO_LOG(GT_MOD_CORE, "\t header %2d: address=%10p", j, addr);
        Dl_info   dlinfo;
        dladdr(addr, &dlinfo);
        GT_INFO_LOG(GT_MOD_CORE, "\t %s:%s", dlinfo.dli_fname, dlinfo.dli_sname);
    }
    return 0;
}
#endif
void gt_signal_routine(int32_t sig)
{
    int32_t save_errno = errno;
    //GT_INFO_LOG(GT_MOD_CORE, "Calltrace: Catch signal %d", sig);
    switch (sig)
    {
        case GT_SIGHUP:
        case GT_SIGQUIT:
        case GT_SIGUSR1:
        case GT_SIGUSR2:
            {
                break;
            }
        case GT_SIGILL:
        case GT_SIGABRT:
        case GT_SIGBUS:
        case GT_SIGFPE:
        case GT_SIGSEGV:
        case GT_SIGSYS:
            {
                gt_backtrace_print(sig);
                break;
            }
        default:
            {
                break;
            }
    }

    gt_signal_deliver(sig);

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

int32_t gt_signal_register_custom(void)
{
    int32_t ret = GT_OK;
    for (int32_t i = 0; i < DIM(g_gt_sig_custom); i++)
    {
        GT_INFO_LOG(GT_MOD_CORE, "Set handle to user sig:%d", g_gt_sig_custom[i]);
        ret = gt_add_signal_handler(g_gt_sig_custom[i], gt_signal_routine);
        if (GT_OK != ret)
        {
            GT_ERROR_LOG(GT_MOD_CORE, "Failed to register user signal handler!");
            return ret;
        }
    }
    return GT_OK;
}

int32_t gt_signal_register_core(void)
{
    int32_t ret = GT_OK;
    for (int32_t i = 0; i < DIM(g_gt_sig_core); i++)
    {
        GT_INFO_LOG(GT_MOD_CORE, "Set handle to user sig:%d", g_gt_sig_core[i]);
        ret = gt_add_signal_handler(g_gt_sig_core[i], gt_signal_routine);
        if (GT_OK != ret)
        {
            GT_ERROR_LOG(GT_MOD_CORE, "Failed to register core signal handler!");
            return ret;
        }
    }
    return GT_OK;
}

int32_t gt_signal_register_ignore(void)
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

int32_t gt_signal_phase1_func(void)
{
    int32_t ret = GT_OK;

    struct gt_event *ge = gt_poller_gevent_create(g_signal_pipefd[0], &g_gt_core_poller);
    if (NULL == ge)
    {
        GT_ERROR_LOG(GT_MOD_CORE, "Failed to create gt_event for signal.");
        return GT_ERROR;
    }
    ge->ops.read = gt_signal_ops_read;

    /* start POLLIN for signal pipefd */
    ret = gt_poller_event_start(EPOLLIN, ge);
    if (GT_OK != ret)
    {
        //GT_ERROR_LOG(GT_MOD_CORE, "Failed to init poller.");
        return ret;
    }
    return GT_OK;
}

int32_t gt_signal_handle_phase(void)
{
    int32_t ret = GT_OK;
    ret = gt_phase_register(GT_CMOD_SIGNAL, GT_PHASE_1, gt_signal_phase1_func);
    if (GT_OK != ret)
    {
        //GT_ERROR_LOG(GT_MOD_CORE, "Failed to register phase.");
        return ret;
    }
    return GT_OK;
}

int32_t gt_signals_init(void)
{
    //int32_t ret = GT_OK;
    /* create signal piepfd */
    gt_signal_create_pipefd();
    /* register user signals */
    gt_signal_register_custom();
    /* handle core signals */
    gt_signal_register_core();
    /* handle Ignore signals */
    gt_signal_register_ignore();
    /* handle Default signals */
    // Nothing to do
    gt_signal_handle_phase();
    return GT_OK;
}
