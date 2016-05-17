
#include "../../../sys/kdef.h"
#include "../../../sys/bit_ops.h"

typedef union {
    uint32_t size;
    uint32_t flags;
} alloc_header;

#define ALLOC_FLAG_NUM 2
#define ALLOC_HEADER_FLAG_FREE 0
#define ALLOC_HEADER_FLAG_LAST 1

static void SAPI _set_alloc_header_size(alloc_header *header, uint32_t size)
{
    // align the integer, ignoring overflowed bits
    size <<= ALLOC_FLAG_NUM;

    // clear ALLOC_FLAG_NUM-th to 31-th bits
    header->size &= ~bit_field_mask_32(ALLOC_FLAG_NUM, 31);
    // set bits
    header->size |= size;
    return;
}

static uint32_t SAPI _read_alloc_header_size(alloc_header *header)
{
    return header->size >> ALLOC_FLAG_NUM;
}

static uint32_t SAPI _read_alloc_header_flag(alloc_header *header, uint32_t bit)
{
    return (header->flags & bit_mask_32(bit)) == 0 ? 0 : 1;
}

static void SAPI _set_alloc_header_flag(alloc_header *header, uint32_t bit, uint32_t value)
{
    value &= bit_mask_32(0);
    if(value == 1)
    {
        header->flags |= bit_mask_32(bit);
    }
    else
    {
        header->flags &= ~bit_mask_32(bit);
    }
    return;
}

static void _alloc_join(void *base)
{
    if(base != NULL)
    {
        char* c_ptr = (char*)base;
        while(1)
        {
            uint32_t c_blk_free = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t c_blk_last = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            uint32_t c_blk_size = _read_alloc_header_size((alloc_header *) c_ptr);
            char* n_ptr = c_blk_last == 1 ? NULL : c_ptr + c_blk_size;
            if(n_ptr != NULL && c_blk_free == 1)
            {
                // if this is not the last block and the prev block is free
                uint32_t n_blk_free = _read_alloc_header_flag((alloc_header *) n_ptr, ALLOC_HEADER_FLAG_FREE);
                uint32_t n_blk_last = _read_alloc_header_flag((alloc_header *) n_ptr, ALLOC_HEADER_FLAG_LAST);
                uint32_t n_blk_size = _read_alloc_header_size((alloc_header *) n_ptr);

                if (n_blk_free == 1)
                {
                    // logically gone
                    _set_alloc_header_size((alloc_header *) c_ptr, n_blk_size + c_blk_size);
                    _set_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST, n_blk_last);
                    continue;
                }
            }
            // update the c_ptr
            if(c_blk_last == 0)
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

//static void salloc_print(void* base)
//{
//    if(base != NULL)
//    {
//        //printf("=======================================================\n");
//        char* c_ptr = (char*)base;
//        while(1)
//        {
//            uint32_t cur_blk_free = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
//            uint32_t cur_blk_last = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
//            uint32_t cur_blk_size = _read_alloc_header_size((alloc_header *) c_ptr);
//            //printf("Block:0x%llX Size:%d Free:%d Last:%d\n", (unsigned long long)c_ptr, cur_blk_size, cur_blk_free, cur_blk_last);
//
//            if(cur_blk_last == 1)
//            {
//                break;
//            }
//            else
//            {
//                c_ptr += cur_blk_size;
//            }
//        }
//    }
//    return;
//}

void salloc_init(void *base, uint32_t size)
{
    if(base != NULL && size >= sizeof(alloc_header))
    {
        alloc_header* ptr = (alloc_header*)base;
        _set_alloc_header_size(ptr, size);
        _set_alloc_header_flag(ptr, ALLOC_HEADER_FLAG_FREE, 1);
        _set_alloc_header_flag(ptr, ALLOC_HEADER_FLAG_LAST, 1);
    }
    return;
}

void* salloc(void *base, uint32_t size)
{
    void* result = NULL;
    if(base != NULL && size != 0)
    {
        uint32_t total_size = size + sizeof(alloc_header);
        char* c_ptr = (char*)base;
        while(1)
        {
            uint32_t cur_blk_free = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t cur_blk_size = _read_alloc_header_size((alloc_header *) c_ptr);
            uint32_t cur_blk_last = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            if(cur_blk_free == 0 || cur_blk_size < total_size)
            {
                //if cur block not a free block
                //or the current block size is less than the size we want
                if(cur_blk_last == 1)
                    //if last one, break and fail.
                    break;
                else
                    c_ptr += cur_blk_size;
            }
            else
            {
                // we have a free block with enough size
                if(total_size == cur_blk_size ||
                   cur_blk_size - total_size < sizeof(alloc_header))
                {
                    // since the space left is not enough for alloc_header
                    // we alloc the whole block
                    _set_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE, 0);
                }
                else
                {
                    // we split the block here
                    // set properties for the first block
                    _set_alloc_header_size((alloc_header *) c_ptr, total_size);
                    _set_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST, 0);
                    _set_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE, 0);

                    // set properties for the second block
                    _set_alloc_header_size((alloc_header *) (c_ptr + total_size), cur_blk_size - total_size);
                    _set_alloc_header_flag((alloc_header *) (c_ptr + total_size), ALLOC_HEADER_FLAG_LAST, cur_blk_last);
                    _set_alloc_header_flag((alloc_header *) (c_ptr + total_size), ALLOC_HEADER_FLAG_FREE, 1);
                }
                // return the pointer, skip the alloc header
                result =  c_ptr + sizeof(alloc_header);
                break;
            }
        }
    }
    return result;
}

void sfree(void *base, void *ptr)
{
    if(base != NULL && ptr != NULL)
    {
        char* c_ptr = (char*)base;
        while(1)
        {
            uint32_t cur_blk_free = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t cur_blk_last = _read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            uint32_t cur_blk_size = _read_alloc_header_size((alloc_header *) c_ptr);
            if (cur_blk_free == 0 && ptr == c_ptr + sizeof(alloc_header))
            {
                // we found the block, mark it as free
                _set_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE, 1);
                // merge blocks
                _alloc_join(base);
                break;
            }

            if(cur_blk_last == 1)
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