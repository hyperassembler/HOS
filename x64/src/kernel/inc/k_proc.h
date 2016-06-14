/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _PROC_H
#define _PROC_H


#include "k_def.h"

typedef struct
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
} __attribute__((packed)) process_context_t;

typedef struct
{
    uint32_t process_id;
    uint32_t priority;
    process_context_t context;

} __attribute__((packed)) process_control_block_t;


#endif
