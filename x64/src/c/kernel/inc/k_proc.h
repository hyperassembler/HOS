//
// Created by Oscar on 2016-04-24.
//

#ifndef _PROC_H
#define _PROC_H


#include "k_def.h"

typedef struct __attribute__((packed))
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
} process_context_t;

typedef struct __attribute__((packed))
{
    uint32_t process_id;
    uint32_t priority;
    process_context_t context;

} process_control_block_t;


#endif
