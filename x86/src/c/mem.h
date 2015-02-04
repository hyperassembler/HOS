#ifndef _MEM_H_
#define _MEM_H_
#include "type.h"
#include "kdef.h"

#define SEG_GRANULARITY ((uint64_t)1 << 55)
#define SEG_LONG ((uint64_t)1 << 53)
#define SEG_DPL(dpl) (((uint64_t)(dpl) & 0x3) << 45)
#define SEG_PRESENT ((uint64_t)1 << 47)
#define SEG_CODE_DATA ((uint64_t)1 << 44)
#define SEG_TYPE(type) (((uint64_t)(type) & 0xF) << 40)
#define SEG_AVAILABLE ((uint64_t)1 << 52)
#define SEG_32_BITS ((uint64_t)1 << 54)

typedef struct __attribute__ ((packed))
{
	uint16_t limit;
	uint32_t base;
} gdt_ptr_t;

typedef struct __attribute__ ((packed))
{
    uint16_t limit;
    uint32_t base;
} idt_ptr_t;

void HYPKERNEL32 hk_write_segment_descriptor(void *const gdt, uint32_t const base, uint32_t const limit, uint64_t const attr);
extern void HYPKERNEL32 hk_load_gdt(gdt_ptr_t const * const ptr, uint16_t const sel_code, uint16_t const sel_data);
void HYPKERNEL32 hk_mem_cpy(void* src, void* dst, uint32_t size);
void HYPKERNEL32 hk_mem_move(void* src, void* dst, uint32_t size);
extern int32_t HYPKERNEL32 hk_support_x64(void);
extern void hk_disable_paging(void);
extern void hk_enable_paging(void);
void HYPKERNEL32 hk_mem_set(void* src, int8_t const val,uint32_t size);
#endif
