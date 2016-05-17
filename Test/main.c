
#include <stdint.h>
#include <stdio.h>
#include "bit_operation.h"

typedef union {
    uint32_t size;
    uint32_t flags;
} alloc_header;

#define ALLOC_FLAG_NUM 2
#define ALLOC_HEADER_FLAG_FREE 0
#define ALLOC_HEADER_FLAG_LAST 1


void set_alloc_header_size(alloc_header* header, uint32_t size)
{
    // align the integer, ignoring overflowed bits
    size <<= ALLOC_FLAG_NUM;

    // clear ALLOC_FLAG_NUM-th to 31-th bits
    header->size &= ~bit_field_mask_32(ALLOC_FLAG_NUM, 31);
    // set bits
    header->size |= size;
    return;
}

uint32_t read_alloc_header_size(alloc_header* header)
{
    return header->size >> ALLOC_FLAG_NUM;
}

uint32_t read_alloc_header_flag(alloc_header* header, uint32_t bit)
{
    return (header->flags & bit_mask_32(bit)) == 0 ? 0 : 1;
}

void set_alloc_header_flag(alloc_header* header, uint32_t bit, uint32_t value)
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

int init_alloc(void* base, unsigned int size)
{
    if(base != NULL && size >= sizeof(unsigned int))
    {
        alloc_header* ptr = (alloc_header*)base;
        set_alloc_header_size(ptr, size);
        set_alloc_header_flag(ptr, ALLOC_HEADER_FLAG_FREE, 1);
        set_alloc_header_flag(ptr, ALLOC_HEADER_FLAG_LAST, 1);
        return 0;
    }
    return 1;
}

void alloc_join_free(void *base)
{
    if(base != NULL)
    {
        char* c_ptr = (char*)base;
        while(1)
        {
            uint32_t c_blk_free = read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t c_blk_last = read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            uint32_t c_blk_size = read_alloc_header_size((alloc_header*)c_ptr);
            char* n_ptr = c_blk_last == 1 ? NULL : c_ptr + c_blk_size;
            if(n_ptr != NULL && c_blk_free == 1)
            {
                // if this is not the last block and the prev block is free
                uint32_t n_blk_free = read_alloc_header_flag((alloc_header *) n_ptr, ALLOC_HEADER_FLAG_FREE);
                uint32_t n_blk_last = read_alloc_header_flag((alloc_header *) n_ptr, ALLOC_HEADER_FLAG_LAST);
                uint32_t n_blk_size = read_alloc_header_size((alloc_header*)n_ptr);

                if (n_blk_free == 1)
                {
                    // logically gone
                    set_alloc_header_size((alloc_header *) c_ptr, n_blk_size + c_blk_size);
                    set_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST, n_blk_last);
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

int kfree(void* base, void* ptr)
{
    if(base != NULL && ptr != NULL)
    {
        char* c_ptr = (char*)base;
        while(1)
        {
            uint32_t cur_blk_free = read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t cur_blk_last = read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            uint32_t cur_blk_size = read_alloc_header_size((alloc_header*)c_ptr);
            if (cur_blk_free == 0 && ptr == c_ptr + sizeof(alloc_header))
            {
                // we found the block, mark it as free
                set_alloc_header_flag((alloc_header*)c_ptr, ALLOC_HEADER_FLAG_FREE, 1);
                // merge blocks
                alloc_join_free(base);
                return 0;
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
    return 1;
}

void kalloc_print(void* base)
{
    if(base != NULL)
    {
        printf("=======================================================\n");
        char* c_ptr = (char*)base;
        while(1)
        {
            uint32_t cur_blk_free = read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t cur_blk_last = read_alloc_header_flag((alloc_header *) c_ptr, ALLOC_HEADER_FLAG_LAST);
            uint32_t cur_blk_size = read_alloc_header_size((alloc_header*)c_ptr);
            printf("Block:0x%llX Size:%d Free:%d Last:%d\n", (unsigned long long)c_ptr, cur_blk_size, cur_blk_free, cur_blk_last);

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

int kalloc(void* base, unsigned int size, void** ptr)
{
    if(base != NULL && size != 0 && ptr != NULL)
    {
        unsigned int total_size = size + sizeof(alloc_header);
        char* c_ptr = (char*)base;
        while(1)
        {
            uint32_t cur_blk_free = read_alloc_header_flag((alloc_header*)c_ptr, ALLOC_HEADER_FLAG_FREE);
            uint32_t cur_blk_size = read_alloc_header_size((alloc_header*)c_ptr);
            uint32_t cur_blk_last = read_alloc_header_flag((alloc_header*)c_ptr, ALLOC_HEADER_FLAG_LAST);
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
                    set_alloc_header_flag((alloc_header*)c_ptr, ALLOC_HEADER_FLAG_FREE, 0);
                }
                else
                {
                    // we split the block here
                    // set properties for the first block
                    set_alloc_header_size((alloc_header*)c_ptr, total_size);
                    set_alloc_header_flag((alloc_header*)c_ptr, ALLOC_HEADER_FLAG_LAST, 0);
                    set_alloc_header_flag((alloc_header*)c_ptr, ALLOC_HEADER_FLAG_FREE, 0);

                    // set properties for the second block
                    set_alloc_header_size((alloc_header*)(c_ptr + total_size), cur_blk_size - total_size);
                    set_alloc_header_flag((alloc_header*)(c_ptr + total_size), ALLOC_HEADER_FLAG_LAST, cur_blk_last);
                    set_alloc_header_flag((alloc_header*)(c_ptr + total_size), ALLOC_HEADER_FLAG_FREE, 1);
                }
                // return the pointer, skip the alloc header
                *ptr =  c_ptr + sizeof(alloc_header);
                return 0;
            }
        }
    }
    // getting here means did not work
    if(ptr != NULL)
        *ptr = NULL;
    return 1;
}

char buffer[1024];


int main()
{
    void *a = NULL, *b = NULL, *c = NULL, *d = NULL;
    init_alloc(buffer, 1024);
    kalloc(buffer, 10, &a);
    kalloc_print(buffer);
    kalloc(buffer, 10, &b);
    kalloc_print(buffer);
    kalloc(buffer, 10, &c);
    kalloc_print(buffer);
    kalloc(buffer, 10, &d);
    kalloc_print(buffer);
    kfree(buffer, b);
    kalloc_print(buffer);
    kfree(buffer,c);
    kalloc_print(buffer);
    kalloc(buffer,20,&b);
    kalloc_print(buffer);
    kfree(buffer,b);
    kalloc_print(buffer);
    kfree(buffer,a);
    kfree(buffer,b);
    kfree(buffer,c);
    kfree(buffer,d);
    kalloc_print(buffer);
    kalloc(buffer, 1024, &a);
    kalloc_print(buffer);
    kalloc(buffer, 1024 - sizeof(alloc_header), &a);
    kalloc_print(buffer);
    kfree(buffer,a);
    kalloc_print(buffer);
    return 0;
}