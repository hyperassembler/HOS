#include "kdef.h"
#include "mem.h"

void NATIVE32 hk_write_segment_descriptor(void * const gdt, uint32_t const base, uint32_t const limit, uint64_t const attr)
{
	if (gdt == NULL)
		return;
    uint64_t const seg_desc = (((uint64_t)base & 0xFFFF) << 16) | ((((uint64_t)base >> 16) & 0xFF) << 32) | ((((uint64_t)base >> 24) & 0xFF) << 56) | ((uint64_t)limit & 0xFFFF) | ((((uint64_t)limit >> 16) & 0xF) << 48) | attr;
    ((uint8_t*)gdt)[0] = (uint8_t)(seg_desc & 0xFF);
    ((uint8_t*)gdt)[1] = (uint8_t)((seg_desc >> 8) & 0xFF);
    ((uint8_t*)gdt)[2] = (uint8_t)((seg_desc >> 16) & 0xFF);
    ((uint8_t*)gdt)[3] = (uint8_t)((seg_desc >> 24) & 0xFF);
    ((uint8_t*)gdt)[4] = (uint8_t)((seg_desc >> 32) & 0xFF);
    ((uint8_t*)gdt)[5] = (uint8_t)((seg_desc >> 40) & 0xFF);
    ((uint8_t*)gdt)[6] = (uint8_t)((seg_desc >> 48) & 0xFF);
    ((uint8_t*)gdt)[7] = (uint8_t)((seg_desc >> 56) & 0xFF);
    return;
}

void NATIVE32 hk_mem_cpy(void* src, void* dst, uint32_t size)
{
    if (src == NULL || dst == NULL)
        return;
    char* cSrc = (char*)src;
    char* cDst = (char*)dst;
    while (size--)
        *(cDst++) = *(cSrc++);
    return;
}

void NATIVE32 hk_mem_set(void* src, int8_t const val,uint32_t size)
{
    if (src == NULL)
        return;
    int8_t * cSrc = (int8_t *)src;
    while (size--)
        *(cSrc++) = val;
    return;
}

void NATIVE32 hk_mem_move(void* src, void* dst, uint32_t size)
{
    if (src == NULL || dst == NULL)
        return;
    char* cSrc = (char*)src;
    char* cDst = (char*)dst;
    if (cSrc >= cDst)
    {
        return hk_mem_cpy(src,dst,size);
    }
    cSrc += size;
    cDst += size;
    while (size--)
        *(--cDst) = *(--cSrc);
    return;
}