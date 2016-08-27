/*-------------------------------------------------------
 |
 |      bifrost_semaphore.h
 |
 |      Contains Bifrost semaphore APIs,
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_SEMAPHORE_H_
#define _BIFROST_SEMAPHORE_H_

#include "linked_list.h"
#include "bifrost_lock.h"
#include "bifrost_statuscode.h"
#include "bifrost_ref.h"

// this library depends on thread_block and thread_resume
// any functions called by thread_block and thread_resume should not use semaphores

typedef struct
{
    hw_ref_node_t ref_node;
    hw_spin_lock_t lock;
    linked_list_t block_list;
    int32_t count;
} hw_sem_t;

typedef struct
{
    linked_list_node_t node;
    void *tcb;
    int32_t quota;
    hw_callback_func_t free_callback;
} hw_sem_node_t;

hw_result_t hw_sem_create(hw_handle_t *out, int32_t count);

hw_result_t hw_sem_wait(hw_handle_t handle, int32_t quota);

hw_result_t hw_sem_signal(hw_handle_t handle, int32_t quota);

hw_result_t hw_sem_trywait(hw_handle_t handle, int32_t quota);


hw_result_t ke_sem_init(hw_sem_t *sem, int32_t count);

hw_result_t ke_sem_signal(hw_sem_t *sem, int32_t quota);

hw_result_t ke_sem_wait(hw_sem_t *sem, hw_sem_node_t *node, int quota);

hw_result_t ke_sem_trywait(hw_sem_t *sem, int32_t quota);

#endif

