#ifndef _KDEF_H_
#define _KDEF_H_

#include <stddef.h>
#include <stdarg.h>

#define NATIVE64 __attribute__((sysv_abi))

#define ALIGN_DOWN(val, alignment) (((val) / (alignment)) * (alignment))

#define ALIGN_UP(val, alignment) ((((val) % (alignment)) == 0) ? (((val) / (alignment)) * (alignment)) : ((((val) / (alignment)) * (alignment)) + 1))

#define IS_OVERLAP(x1,x2,y1,y2) (((x1) <= (y2)) && ((y1) <= (x2)))

#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define SEG_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#endif
