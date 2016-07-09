/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "balloc.h"
#include "std_lib.h"
#include "bit_ops.h"

int32_t balloc_init(balloc_desc_t *desc,
                    uint64_t base,
                    uint64_t end,
                    uint64_t page_size,
                    balloc_alloc_func alloc,
                    balloc_free_func free,
                    balloc_lock_func lock,
                    balloc_unlock_func unlock)
{
    if(desc == NULL || base >= end || page_size == 0 || alloc == 0
            || free == 0 || lock == 0 || unlock == 0)
        return BALLOC_STATUS_INVALID_ARGUMENTS;

    if(((end - base) % page_size) != 0)
        return BALLOC_STATUS_INVALID_ALIGNMENT;

    desc->alloc = alloc;
    desc->free = free;
    desc->lock = lock;
    desc->unlock = unlock;

    desc->base = base;
    desc->page_size = page_size;
    uint64_t quot = (end-base) / page_size;
    uint32_t order = log_base_2(quot);
    if(quot & bit_mask_64(order) != 0)
    {
        order++;
    }
    desc->order = order;

    // allocate linked lists and bit maps
    desc->free_lists = (linked_list_t*)desc->alloc((order + 1) * sizeof(linked_list_t));
    if(desc->free_lists == NULL || desc->bit_map == NULL)
        return BALLOC_STATUS_CANT_ALLOC_MEM;
    
    return BALLOC_STATUS_SUCCESS;
}

int32_t balloc_alloc(balloc_desc_t* desc, uint32_t page_num, uint64_t* out)
{

}

int32_t balloc_free(balloc_desc_t* desc, uint64_t addr)
{

}

int32_t balloc_mark_used(balloc_desc_t* desc, uint64_t start, uint64_t end)
{

}

int32_t balloc_mark_free(balloc_desc_t* desc, uint64_t start, uint64_t end)
{

}