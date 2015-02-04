#ifndef _MEM_H_
#define _MEM_H_

#include "type.h"
#include "kdef.h"
#define PML4_PRESENT ((uint64_t)1 << 0)
#define PML4_WRITE ((uint64_t)1 << 1)
#define PML4_USER ((uint64_t)1 << 2)
#define PML4_WRITE_THROUGH ((uint64_t)1 << 3)
#define PML4_CACHE_DISABLED ((uint64_t)1 << 4)
#define PML4_ACCESSED ((uint64_t)1 << 5)
#define PML4_EXECUTION_DISABLED ((uint64_t)1 << 63)

#define PDPT_PRESENT ((uint64_t)1 << 0)
#define PDPT_WRITE ((uint64_t)1 << 1)
#define PDPT_USER ((uint64_t)1 << 2)
#define PDPT_WRITE_THROUGH ((uint64_t)1 << 3)
#define PDPT_CACHE_DISABLED ((uint64_t)1 << 4)
#define PDPT_ACCESSED ((uint64_t)1 << 5)
#define PDPT_EXECUTION_DISABLED ((uint64_t)1 << 63)

#define PD_PRESENT ((uint64_t)1 << 0)
#define PD_WRITE ((uint64_t)1 << 1)
#define PD_USER ((uint64_t)1 << 2)
#define PD_WRITE_THROUGH ((uint64_t)1 << 3)
#define PD_CACHE_DISABLED ((uint64_t)1 << 4)
#define PD_ACCESSED ((uint64_t)1 << 5)
#define PD_EXECUTION_DISABLED ((uint64_t)1 << 63)

#define PT_PRESENT ((uint64_t)1 << 0)
#define PT_WRITE ((uint64_t)1 << 1)
#define PT_USER ((uint64_t)1 << 2)
#define PT_WRITE_THROUGH ((uint64_t)1 << 3)
#define PT_CACHE_DISABLED ((uint64_t)1 << 4)
#define PT_ACCESSED ((uint64_t)1 << 5)
#define PT_DIRTY ((uint64_t)1 << 6)
#define PT_ATTRIBUTE_TABLE ((uint64_t)1 << 7)
#define PT_GLOBAL ((uint64_t)1 << 8)
#define PT_EXECUTION_DISABLED ((uint64_t)1 << 63)

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
    uint64_t base;
} gdt_ptr_t;

typedef struct __attribute__ ((packed))
{
    uint16_t limit;
    uint64_t base;
} idt_ptr_t;

void HYPKERNEL64 hk_write_segment_descriptor(void *const gdt, uint32_t const base, uint32_t const limit, uint64_t const attr);

//extern void HYPKERNEL64 hk_load_gdt(gdt_ptr_t const *const ptr, uint16_t const sel_code, uint16_t const sel_data);

void HYPKERNEL64 hk_mem_cpy(void *src, void *dst, uint64_t size);

void HYPKERNEL64 hk_mem_move(void *src, void *dst, uint64_t size);

void HYPKERNEL64 hk_mem_set(void *src, int8_t const val, uint64_t size);

void HYPKERNEL64 hk_write_pml4_entry(void *const base, uint64_t const pdpt_addr, uint64_t const attr);

void HYPKERNEL64 hk_write_pdpt_entry(void *const base, uint64_t const pd_addr, uint64_t const attr);

void HYPKERNEL64 hk_write_pd_entry(void *const base, uint64_t const pt_addr, uint64_t const attr);

void HYPKERNEL64 hk_write_pt_entry(void *const base, uint64_t const p_addr, uint64_t const attr);

#endif
