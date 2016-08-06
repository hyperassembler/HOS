#ifndef _K_REF_H_
#define _K_REF_H_

#include "g_abi.h"
#include "g_type.h"
#include "avl_tree.h"
#include "k_atomic.h"

typedef void (*k_ref_callback_func_t)(void* ptr, void* context);

typedef struct
{
    avl_tree_t* avl_tree;
    _Bool initialized;
    k_spin_lock_t lock;
} k_ref_desc_t;

#define K_REF_STATUS_SUCCESS 0
#define K_REF_STATUS_INVALID_ARGUMENTS 1
#define K_REF_STATUS_CANNOT_ALLOCATE_MEM 2
#define K_REF_STATUS_REF_NOT_FOUND 3

int32_t KAPI k_ref_init(k_ref_desc_t* desc);

int32_t KAPI k_ref_create(void* ptr, k_ref_callback_func_t callback, void* context);

int32_t KAPI k_ref_inc(void* ptr);

int32_t KAPI k_ref_dec(void* ptr);

#endif