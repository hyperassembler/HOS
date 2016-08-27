#ifndef _BIFROST_STDLIB_H
#define _BIFROST_STDLIB_H

#include "bifrost_types.h"

int32_t hw_memcmp(const void *ptr1, const void *ptr2, const size_t len);

void hw_memset(void *ptr, uint8_t value, size_t len);

#define OBTAIN_STRUCT_ADDR(member_addr, member_name, struct_name) ((struct_name*)((char*)(member_addr)-(char*)(&(((struct_name*)0)->member_name))))

#endif
