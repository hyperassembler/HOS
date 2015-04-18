#ifndef _KDEF_H_
#define _KDEF_H_

#include <stddef.h>

#define NATIVE64 __attribute__((sysv_abi))

#define SEG_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#endif
