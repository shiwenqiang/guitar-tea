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
typedef int8_t              gt_int8_t;
typedef int16_t             gt_int16_t;
typedef int32_t             gt_int32_t;
typedef int64_t             gt_int64_t;

typedef int32_t             gt_int_t;
/* Unsigned */
typedef uint8_t             gt_uint8_t;
typedef uint16_t            gt_uint16_t;
typedef uint32_t            gt_uint32_t;
typedef uint64_t            gt_uint64_t;

typedef uint32_t            gt_uint_t;

/* Specific */


/* Macro Define Section */

#define GT_TRUE             (1)
#define GT_FALSE            (0)

#define GT_OK               (0)
#define GT_ERROR            (-1)

#define DIM(__arr)  (sizeof(__arr)/sizeof(__arr[0]))

/* Structure Define Section */

/* Prototype Declare Section */


#endif /* __GT_TYPEDEF_H__ */
