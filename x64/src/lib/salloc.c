/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "abi.h"
#include "type.h"
#include "lib/sxtdlib.h"

typedef union
{
    uint32_t size;
    uint32_t flags;
} _salloc_header;

#define ALLOC_FLAG_NUM 2
#define ALLOC_HEADER_FLAG_FREE 0
#define ALLOC_HEADER_FLAG_LAST 1

static void lbp_set_salloc_header_size(_salloc_header *header, uint32_t size)
{
    // align the integer, ignoring overflowed bits
    size <<= ALLOC_FLAG_NUM;

    // clear ALLOC_FLAG_NUM-th to 31-th bits
    header->size &= ~(uint32_t) lb_bit_field_mask(ALLOC_FLAG_NUM, 31);
    // set bits
    header->size |= size;
    return;
}

static uint32_t lbp_read_salloc_header_size(_salloc_header *header)
{
    return header->size >> ALLOC_FLAG_NUM;
}

static uint32_t lbp_read_salloc_header_flag(_salloc_header *header, uint32_t bit)
{
    return (header->flags & (uint32_t) lb_bit_mask(bit)) == 0 ? 0 : 1;
}

static void lbp_set_salloc_header_flag(_salloc_header *header, uint32_t bit, uint32_t value)
{
    value &= (uint32_t) lb_bit_mask(0);
    if (value == 1)
    {
        header->flags |= (uint32_t) lb_bit_mask(bit);
    }
    else
    {
        header->flags &= ~(uint32_t) lb_bit_mask(bit);
    }
    return;
}

static void lbp_salloc_join(void *base)
{
    if (base != NULL)
    {
        char *c_ptr = (char *) base;
        while (1)
        {
            uint32_t c_blk_free = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t c_blk_last = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            uint32_t c_blk_size = lbp_read_salloc_header_size((_salloc_header *) c_ptr);
            char *n_ptr = c_blk_last == 1 ? NULL : c_ptr + c_blk_size;
            if (n_ptr != NULL && c_blk_free == 1)
            {
                // if this is not the last block and the prev block is free
                uint32_t n_blk_free = lbp_read_salloc_header_flag((_salloc_header *) n_ptr, ALLOC_HEADER_FLAG_FREE);
                uint32_t n_blk_last = lbp_read_salloc_header_flag((_salloc_header *) n_ptr, ALLOC_HEADER_FLAG_LAST);
                uint32_t n_blk_size = lbp_read_salloc_header_size((_salloc_header *) n_ptr);

                if (n_blk_free == 1)
                {
                    // logically gone
                    lbp_set_salloc_header_size((_salloc_header *) c_ptr, n_blk_size + c_blk_size);
                    lbp_set_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST, n_blk_last);
                    continue;
                }
            }
            // update the c_ptr
            if (c_blk_last == 0)
            {
                c_ptr += c_blk_size;
            }
            else
            {
                break;
            }
        }
    }
}

bool KABI lb_salloc_assert(void *base, uint32_t *blk_size, bool *blk_free, uint32_t size)
{
    if (base == NULL || blk_free == NULL || blk_size == NULL)
    {
        return NULL;
    }
    uint32_t i = 0;
    char *c_ptr = (char *) base;
    while (1)
    {
        uint32_t cur_blk_free = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
        uint32_t cur_blk_last = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
        uint32_t cur_blk_size = lbp_read_salloc_header_size((_salloc_header *) c_ptr);
        if (cur_blk_free != blk_free[i] || cur_blk_size != blk_size[i])
        {
            return false;
        }
        else
        {
            c_ptr += cur_blk_size;
            i++;
        }
        if (cur_blk_last == 1)
        {
            return i == size;
        }
    }
}

void KABI lb_salloc_init(void *base, uint32_t size)
{
    if (base != NULL && size >= sizeof(_salloc_header))
    {
        _salloc_header *ptr = (_salloc_header *) base;
        lbp_set_salloc_header_size(ptr, size);
        lbp_set_salloc_header_flag(ptr, ALLOC_HEADER_FLAG_FREE, 1);
        lbp_set_salloc_header_flag(ptr, ALLOC_HEADER_FLAG_LAST, 1);
    }
    return;
}

void *KABI lb_salloc(void *base, uint32_t size)
{
    void *result = NULL;
    if (base != NULL && size != 0)
    {
        uint32_t total_size = size + sizeof(_salloc_header);
        char *c_ptr = (char *) base;
        while (1)
        {
            uint32_t cur_blk_free = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t cur_blk_size = lbp_read_salloc_header_size((_salloc_header *) c_ptr);
            uint32_t cur_blk_last = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            if (cur_blk_free == 0 || cur_blk_size < total_size)
            {
                //if cur block not a free block
                //or the current block size is less than the size we want
                if (cur_blk_last == 1)
                {
                    //if last one, break and fail.
                    break;
                }
                else
                {
                    c_ptr += cur_blk_size;
                }
            }
            else
            {
                // we have a free block with enough size
                if (total_size == cur_blk_size ||
                    cur_blk_size - total_size < sizeof(_salloc_header))
                {
                    // since the space left is not enough for salloc_header
                    // we alloc the whole block
                    lbp_set_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE, 0);
                }
                else
                {
                    // we split the block here
                    // set properties for the first block
                    lbp_set_salloc_header_size((_salloc_header *) c_ptr, total_size);
                    lbp_set_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST, 0);
                    lbp_set_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE, 0);

                    // set properties for the second block
                    lbp_set_salloc_header_size((_salloc_header *) (c_ptr + total_size), cur_blk_size - total_size);
                    lbp_set_salloc_header_flag((_salloc_header *) (c_ptr + total_size), ALLOC_HEADER_FLAG_LAST,
                                               cur_blk_last);
                    lbp_set_salloc_header_flag((_salloc_header *) (c_ptr + total_size), ALLOC_HEADER_FLAG_FREE, 1);
                }
                // return the pointer, skip the alloc header
                result = c_ptr + sizeof(_salloc_header);
                break;
            }
        }
    }
    return result;
}

void KABI lb_sfree(void *base, void *ptr)
{
    if (base != NULL && ptr != NULL)
    {
        char *c_ptr = (char *) base;
        while (1)
        {
            uint32_t cur_blk_free = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t cur_blk_last = lbp_read_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            uint32_t cur_blk_size = lbp_read_salloc_header_size((_salloc_header *) c_ptr);
            if (cur_blk_free == 0 && ptr == c_ptr + sizeof(_salloc_header))
            {
                // we found the block, mark it as free
                lbp_set_salloc_header_flag((_salloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE, 1);
                // merge blocks
                lbp_salloc_join(base);
                break;
            }

            if (cur_blk_last == 1)
            {
                break;
            }
            else
            {
                c_ptr += cur_blk_size;
            }
        }
    }
    return;
}