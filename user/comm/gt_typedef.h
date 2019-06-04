/**************************************************************
 *          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
 *                     All Rights Reseved
 *
 * File     :gt_typedef.h
 * Module   :Log Engine
 * Date     :2015/11/21
 * Author   :shiwenqiang
 * Brief    :Implementation the log.
 *
 *------------------------Revision History---------------------
 *  No. Version Date        Mender        Desc.
 *  1   V0.01   2015-11-22  shiwenqiang   Basic Data Type Orig-
 *                                         inal Version.
 **************************************************************/
#ifndef __GT_TYPEDEF_H__
#define __GT_TYPEDEF_H__

/* Debug Section */

/* Include Files Section */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#include <unistd.h>

/* Basic Type Section */
/* Signed */
typedef int8_t              s8;
typedef int16_t             s16;
typedef int32_t             s32;
typedef int64_t             s64;

/* Unsigned */
typedef uint8_t             u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;

/* Specific */


/* Macro Define Section */

#define GT_TRUE             (1)
#define GT_FALSE            (0)

#define GT_OK               (0)
#define GT_ERROR            (-1)

#define DIM(__arr)  (sizeof(__arr)/sizeof(__arr[0])) //__arr must be a array, not a pointer!

#define likely(x)  __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)
/* Structure Define Section */

/* Prototype Declare Section */


#endif /* __GT_TYPEDEF_H__ */
