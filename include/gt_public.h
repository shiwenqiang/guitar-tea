#ifndef __GT_PUBLIC_H__
#define __GT_PUBLIC_H__

#ifdef __GT_UT__

/* file I/O */

#else /* USR */

/* file I/O */
#include <fcntl.h>
#define gt_open(pathname, oflag, mode...)       open(pathname, oflag, ##mode)
#define gt_creat(pathname, mode)                creat(pathname, mode)
#define gt_close(fd)                            close(fd)
#define gt_lseek(fd, offset, whence)            lseek(fd, offset, whence)


#endif

#endif
