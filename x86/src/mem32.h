#ifndef _MEM_32_H_
#define _MEM_32_H_
#include "type32.h"
#include "io32.h"

#pragma push()
#pragma pack(1)
typedef struct
{
    uint16 limit_low;
    uint16 base_low;
    uint8 base_middle;
    uint8 access;
    uint8 limit_mid_flags;
    uint8 base_high;
} segment_descriptor;

typedef struct
{
	uint16 limit;
	uint32 base;
} gdt_ptr;

typedef struct
{
	
} pde_32;

typedef struct
{
	
} pte_32;


#pragma pop()

int32 hk_set_segment_descriptor(segment_descriptor* pseg_descriptor, uint32 base, uint32 limit, uint8 access, uint8 flags);

#endif