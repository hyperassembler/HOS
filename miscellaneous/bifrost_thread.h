/*-------------------------------------------------------
 |
 |      bifrost_thread.h
 |
 |      Contains Bifrost threading APIs,
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#pragma once

#include "linked_list.h"
#include "bifrost_sim.h"
#include "avl_tree.h"

#define STATE_NUM (5)

typedef enum {
    NEW = 0 ,
    BLOCK = 1,
    RUN = 4,
    READY = 3,
    EXIT = 2
} hw_thread_state_t;

#define PRIORITY_LEVEL_NUM (5)

typedef enum
{
    HIGHEST = 0,
    HIGH = 1,
    MEDIUM = 2,
    LOW = 3,
    LOWEST = 4,
} hw_thread_priority_t;

typedef struct
{
    UINT32 pc;
    UINT32 sp;
    UINT32 a2;
    UINT32 a3;
    UINT32 a4;
    UINT32 a5;
    UINT32 a6;
    UINT32 a7;
    UINT32 a8;
    UINT32 a9;
    UINT32 a10;
    UINT32 a11;
    UINT32 a12;
    UINT32 a13;
    UINT32 a14;
    UINT32 a15;
    UINT32 ps;
    UINT32 epc;
} __attribute__((packed)) hw_regs_t;

typedef struct {
    linked_list_node_t list_node;
    avl_tree_node_t tree_node;
    UINT32 thread_id;
    UINT32 core_id;
    hw_thread_priority_t priority;
    void* stack_ptr;
    UINT32 stack_size;
    hw_regs_t regs;
    void (*proc)(void*);
    void* args;
    hw_thread_state_t state;
    INT32 exit_code;
    HW_MAILBOX mailbox;
    _Bool initialized;
} hw_tcb_t;

int hw_thread_create(void (*proc)(void*),
                     void* args,
                     hw_thread_priority_t priority,
                     UINT32 stack_size,
                     UINT32* thread_id);

int hw_thread_start(UINT32 thread_id);

int hw_thread_block(UINT32 thread_id);

int hw_thread_resume(UINT32 thread_id);

int hw_thread_destroy(UINT32 thread_id);

int hw_thread_get_exit_code(UINT32 thread_id, INT32 *exit_code);

void hw_thread_exit(INT32 exit_code);

void hw_thread_init();

