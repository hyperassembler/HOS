#ifndef _TALLOC_H_
#define _TALLOC_H_

#include "avl_tree.h"

/*
 * A tree allocator for page-size allocation
 * All
 */

#define TALLOC_STATUS_SUCCESS 0
#define TALLOC_STATUS_INVALID_ARGUMENTS 1
#define TALLOC_STATUS_NOT_ENOUGH_MEM 2
#define TALLOC_STATUS_CANT_ALLOC_NODE 3
#define TALLOC_STATUS_INVALID_REIGON 4

typedef void *(*talloc_alloc_func)(uint64_t size);
typedef void (*talloc_free_func)(void* ptr);
typedef void (*talloc_lock_func)(void);
typedef void (*talloc_unlock_func)(void);

typedef struct
{
    avl_tree_t used_base_tree;
    avl_tree_t free_size_tree;
    avl_tree_t free_start_tree;
    avl_tree_t free_end_tree;

    uint64_t page_size;

    talloc_alloc_func alloc;
    talloc_free_func free;
    talloc_lock_func lock;
    talloc_unlock_func unlock;
} talloc_desc_t;


int32_t talloc(talloc_desc_t* desc,
                       uint64_t size,
                       uint64_t alignment,
                       void** out);


int32_t talloc_init(talloc_desc_t *desc,
                    void *base,
                    uint64_t size,
                    uint64_t page_size,
                    talloc_alloc_func alloc,
                    talloc_free_func free,
                    talloc_lock_func lock,
                    talloc_unlock_func unlock);

int32_t talloc_mark_used(talloc_desc_t *desc,
                         void *base,
                         uint64_t size);

int32_t talloc_mark_free(talloc_desc_t* desc,
                         void *base,
                         uint64_t size);

int32_t tfree(talloc_desc_t *desc,
              void *base);

#endif