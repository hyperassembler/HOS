#ifndef _UTIL_H_
#define _UTIL_H_
#include "io32.h"
#include "kdef32.h"

INT32 HKA32 HkClearBit32(void* dst, UINT32 bit);
INT32 HKA32 HkGetBit32(void* dst, UINT32 bit);
INT32 HKA32 HkToggleBit32(void* dst, UINT32 bit);
INT32 HKA32 HkMemcpy(void* src, void* dst, UINT32 size);
INT32 HKA32 HkMemmove(void* src, void* dst, UINT32 size);

#endif 