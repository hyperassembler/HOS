#ifndef _HAL_MEM_H_
#define _HAL_MEM_H_

#include "../type.h"
#include "../kdef.h"

void NATIVE64 mem_copy(void *src, void *dst, uint64_t size);

void NATIVE64 mem_move(void *src, void *dst, uint64_t size);

void NATIVE64 mem_set(void *src, int8_t const val, uint64_t size);

#endif