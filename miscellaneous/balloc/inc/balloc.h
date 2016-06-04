#ifndef _BALLOC_H_
#define _BALLOC_H_

#include "linked_list.h"
#include "k_def.h"
#include "bit_ops.h"
#include "k_type.h"

typedef struct
{
    void* base;
    uint32_t size;
    uint32_t granularity;
    linked_list_t* free_list;
    uint32_t* bit_map;
} balloc_desc_t;



#endif