#ifndef _MEM_32_H_
#define _MEM_32_H_
#include "type32.h"
#include "kdef32.h"

typedef struct __attribute__ ((packed))
{
    uint32 base;
    uint32 limit;
    uint8 type;
    uint8 DPL;
    uint8 Gr;
    uint8 Avl;
    uint8 Pr;
    uint8 Sz; //32 bits = 1, 16 bits = 0
    uint8 x64;
    uint8 Sys; //System = 0, code/data = 1

} segment_descriptor;

typedef struct __attribute__ ((packed))
{
	uint16 limit;
	uint32 base;
} gdt_ptr;

int32 HYPKERNEL32 hk_set_segment_descriptor(uint8* const gdt, const segment_descriptor* const seg_desc);
extern void hk_load_gdt(gdt_ptr* ptr);

#endif
