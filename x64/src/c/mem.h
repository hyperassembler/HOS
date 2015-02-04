#ifndef _MEM_H_
#define _MEM_H_

#include "type.h"
#include "kdef.h"
#define PML4_PRESENT (1ull << 0)
#define PML4_WRITE (1ull << 1)
#define PML4_USER (1ull << 2)
#define PML4_WRITE_THROUGH (1ull << 3)
#define PML4_CACHE_DISABLED (1ull << 4)
#define PML4_ACCESSED (1ull << 5)
#define PML4_EXECUTION_DISABLED (1ull << 63)

#define PDPT_PRESENT (1ull << 0)
#define PDPT_WRITE (1ull << 1)
#define PDPT_USER (1ull << 2)
#define PDPT_WRITE_THROUGH (1ull << 3)
#define PDPT_CACHE_DISABLED (1ull << 4)
#define PDPT_ACCESSED (1ull << 5)
#define PDPT_EXECUTION_DISABLED (1ull << 63)

#define PD_PRESENT (1ull << 0)
#define PD_WRITE (1ull << 1)
#define PD_USER (1ull << 2)
#define PD_WRITE_THROUGH (1ull << 3)
#define PD_CACHE_DISABLED (1ull << 4)
#define PD_ACCESSED (1ull << 5)
#define PD_EXECUTION_DISABLED (1ull << 63)

#define PT_PRESENT (1ull << 0)
#define PT_WRITE (1ull << 1)
#define PT_USER (1ull << 2)
#define PT_WRITE_THROUGH (1ull << 3)
#define PT_CACHE_DISABLED (1ull << 4)
#define PT_ACCESSED (1ull << 5)
#define PT_DIRTY (1ull << 6)
#define PT_ATTRIBUTE_TABLE (1ull << 7)
#define PT_GLOBAL (1ull << 8)
#define PT_EXECUTION_DISABLED (1ull << 63)

#define SEG_GRANULARITY (1ull << 55)
#define SEG_LONG (1ull << 53)
#define SEG_DPL_0 (0ull << 45)
#define SEG_DPL_1 (1ull << 45)
#define SEG_DPL_2 (2ull << 45)
#define SEG_DPL_3 (3ull << 45)
#define SEG_PRESENT (1ull << 47)
#define SEG_CODE_DATA (1ull << 44)
#define SEG_TYPE_DATA_RW (2ull << 40)
#define SEG_TYPE_DATA_R (0ull << 40)
#define SEG_TYPE_CODE_X (8ull << 40)
#define SEG_TYPE_CODE_XR (10ull << 40)
#define SEG_TYPE_CODE_XC (12ull << 40)
#define SEG_TYPE_CODE_XRC (14ull << 40)
#define SEG_AVAILABLE (1ull << 52)
#define SEG_32_BITS (1ull << 54)

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
