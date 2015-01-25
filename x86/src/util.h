#ifndef _UTIL_H_
#define _UTIL_H_
#include "io32.h"
#include "kdef32.h"

int32 HYPKERNEL32 hk_clear_bit(void* dst, uint32 bit);
int32 HYPKERNEL32 hk_get_bit(void* dst, uint32 bit);
int32 HYPKERNEL32 hk_toggle_bit(void* dst, uint32 bit);
int32 HYPKERNEL32 hk_memcpy(void* src, void* dst, uint32 size);
int32 HYPKERNEL32 hk_memmove(void* src, void* dst, uint32 size);

#endif 