#ifndef _KDEF_H_
#define _KDEF_H_

#include <stddef.h>
#include <stdarg.h>

#define _KERNEL_ABI __attribute__((sysv_abi))

#define ALIGN_DOWN(val, alignment) (((val) / (alignment)) * (alignment))

#define ALIGN_UP(val, alignment) ((((val) % (alignment)) == 0) ? (((val) / (alignment)) * (alignment)) : ((((val) / (alignment)) * (alignment)) + 1))

#define IS_OVERLAP(x1,x2,y1,y2) (((x1) <= (y2)) && ((y1) <= (x2)))

#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define SEG_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#define _IN_OPT
#define _OUT_OPT
#define _IN
#define _OUT

typedef enum {
    KERNEL_STATUS_SUCCESS = 0,
    KERNEL_STATUS_FAIL = 1
} kernel_status_t;

#endif
