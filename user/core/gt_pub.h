/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_pub.h
 * Module   :Log Engine
 * Date     :2015/11/22
 * Author   :shiwenqiang
 * Brief    :Implementation the log.
 *
 *------------------------Revision History----------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2015-11-22  shiwenqiang   Original Version.
 **************************************************************/
#ifndef __GT_PUB__
#define __GT_PUB__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "../comm/gt_typedef.h"
#include "../comm/gt_module.h"

extern int32_t gt_create_pipefd(int32_t pipefd[2]);
extern int32_t gt_set_nonblock(int32_t fd);
extern int32_t gt_set_cloexec(int32_t fd);

#endif /* __GT_PUB__ */
