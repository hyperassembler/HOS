#pragma once
#define OBTAIN_STRUCT_ADDR(member_addr, member_name, struct_name) ((struct_name*)((char*)(member_addr)-(char*)(&(((struct_name*)0)->member_name))))

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <bifrost_types.h>


typedef void HW_FUNC();
typedef void HW_RUN_FUNC(uint32_t iteration);

typedef struct {
    HW_FUNC* pinit;
    HW_RUN_FUNC* prunTest;
    HW_FUNC* pcleanup;
    const char* testPathName;
    uint32_t testPathHash;
} HW_XLIST;

#define HW_XLIST_TERMINATION (0xDEADBEEFul)

