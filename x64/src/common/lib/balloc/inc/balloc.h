#ifndef _BALLOC_H_
#define _BALLOC_H_

#include "linked_list.h"
#include "avl_tree.h"

/*
 * A tree allocator for page-size allocation
 * All
 */

#define BALLOC_STATUS_SUCCESS 0
#define BALLOC_STATUS_INVALID_ARGUMENTS 1
#define BALLOC_STATUS_NO_AVAILABLE_MEM 2
#define BALLOC_STATUS_CANT_ALLOC_MEM 3
#define BALLOC_STATUS_INVALID_ALIGNMENT 4

typedef void *(*balloc_alloc_func)(uint64_t size);

typedef void (*balloc_free_func)(void *ptr);

typedef void (*balloc_lock_func)(void);

typedef void (*balloc_unlock_func)(void);

typedef struct
{
    uint64_t base;
    uint64_t order;
    uint64_t page_size;

    linked_list_t *free_lists;
    uint8_t *bit_map;

    balloc_alloc_func alloc;
    balloc_free_func free;
    balloc_lock_func lock;
    balloc_unlock_func unlock;
} balloc_desc_t;


int32_t balloc_init(balloc_desc_t *desc,
                    uint64_t base,
                    uint64_t end,
                    uint64_t page_size,
                    balloc_alloc_func alloc,
                    balloc_free_func free,
                    balloc_lock_func lock,
                    balloc_unlock_func unlock);

int32_t balloc_alloc(balloc_desc_t* desc, uint32_t page_num, uint64_t* out);

int32_t balloc_free(balloc_desc_t* desc, uint64_t addr);

int32_t balloc_mark_used(balloc_desc_t* desc, uint64_t start, uint64_t end);

int32_t balloc_mark_free(balloc_desc_t* desc, uint64_t start, uint64_t end);

#endif