/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_signal.h
 * Module   :Signal Engine
 * Date     :2015/11/21
 * Author   :shiwenqiang
 * Brief    :Implementation the signal.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2017-10-12  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_SIGNAL_H__
#define __GT_SIGNAL_H__

/* The necessary header files */
#include <signal.h>

#define GT_BACKTRACE_BUF_SIZE (100)

typedef enum gt_sig
{
    GT_SIGHUP   = SIGHUP,       /* User */
    GT_SIGINT   = SIGINT,       /* Ignore */
    GT_SIGQUIT  = SIGQUIT,      /* User */
    GT_SIGILL   = SIGILL,       /* Core */
    GT_SIGTRAP  = SIGTRAP,      /* Default */
    GT_SIGABRT  = SIGABRT,      /* Core */
    GT_SIGIOT   = SIGIOT,       /* Ignore */
    GT_SIGBUS   = SIGBUS,       /* Core */
    GT_SIGFPE   = SIGFPE,       /* Core */
    GT_SIGKILL  = SIGKILL,      /* Default */
    GT_SIGUSR1 = SIGUSR1,       /* User */
    GT_SIGSEGV  = SIGSEGV,      /* Core */
    GT_SIGUSR2  = SIGUSR2,      /* User */
    GT_SIGPIPE  = SIGPIPE,      /* Ignore */
    GT_SIGALRM  = SIGALRM,      /* Ignore */
    GT_SIGTERM  = SIGTERM,      /* Default */
    GT_SIGSTKFLT = SIGSTKFLT,   /* Ignore */
    GT_SIGCLD   = SIGCLD,       /* Ignore */
    GT_SIGCHLD  = SIGCHLD,      /* Ignore */
    GT_SIGCONT  = SIGCONT,      /* Ignore */
    GT_SIGSTOP  = SIGSTOP,      /* Default */
    GT_SIGTSTP  = SIGTSTP,      /* Ignore */
    GT_SIGTTIN  = SIGTTIN,      /* Ignore */
    GT_SIGTTOU  = SIGTTOU,      /* Ignore */
    GT_SIGURG   = SIGURG,       /* Default */
    GT_SIGXCPU  = SIGXCPU,      /* Ignore */
    GT_SIGXFSZ  = SIGXFSZ,      /* Ignore */
    GT_SIGVTALRM = SIGVTALRM,   /* Ignore */
    GT_SIGPROF  = SIGPROF,      /* Ignore */
    GT_SIGWINCH = SIGWINCH,     /* Ignore */
    GT_SIGPOLL  = SIGPOLL,      /* Ignore */
    GT_SIGIO    = SIGIO,        /* Ignore */
    GT_SIGPWR   = SIGPWR,       /* Ignore */
    GT_SIGSYS   = SIGSYS,       /* Core */
    GT_SIGUNUSED = SIGUNUSED,   /* Ignore */
}gt_sig_e;

/* Declare Function-pointer's Prototype */
typedef void (*sa_handler_fn) (int32_t sig);

/* Declare Funcions */
extern int32_t gt_signals_init(void);

#endif
