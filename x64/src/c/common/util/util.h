#ifndef _UTIL_H_
#define _UTIL_H_
#include "../sys/type.h"
#define OBTAIN_STRUCT_ADDR(member_addr, member_name, struct_name) ((struct_name*)((char*)(member_addr)-(uint64_t)(&(((struct_name*)0)->member_name))))

#endif