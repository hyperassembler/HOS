#ifndef _LALLOC_H_
#define _LALLOC_H_

#include "linked_list.h"

#define LALLOC_STATUS_SUCCESS 0
#define LALLOC_STATUS_INVALID_ARGUMENTS 1
#define LALLOC_STATUS_NOT_ENOUGH_MEM 2
#define LALLOC_STATUS_CANT_ALLOC_NODE 3
#define LALLOC_STATUS_INVALID_REIGON 4

typedef void *(*lalloc_alloc_func)(uint64_t size);
typedef void (*lalloc_free_func)(void* ptr);

typedef struct
{
    linked_list_t free_list;
    linked_list_t used_list;
    lalloc_alloc_func alloc;
    lalloc_free_func free;
} lalloc_desc_t;

int32_t lalloc_aligned(lalloc_desc_t* desc,
                       uint64_t size,
                       uint64_t alignment,
                       void** out);

int32_t lalloc_set_alloc(lalloc_desc_t* desc, lalloc_alloc_func alloc);

int32_t lalloc_set_free(lalloc_desc_t* desc, lalloc_free_func free);

int32_t lalloc(lalloc_desc_t* desc,
               uint64_t size,
               void** out);

int32_t lalloc_init(lalloc_desc_t *desc,
                    void *base,
                    uint64_t size,
                    lalloc_alloc_func alloc,
                    lalloc_free_func free);

int32_t lalloc_mark_used(lalloc_desc_t *desc,
                         void *base,
                         uint64_t size);

int32_t lalloc_mark_free(lalloc_desc_t* desc,
                         void *base,
                         uint64_t size);

int32_t lfree(lalloc_desc_t *desc,
              void *base);

#endif