#ifndef _MEM_H_
#define _MEM_H_

#include "type.h"
#include "kdef.h"

typedef struct __attribute__ ((packed))
{
    uint64_t Pr;
    uint64_t RW;
    uint64_t USU;
    uint64_t PWT;
    uint64_t PCD;
    uint64_t Acc;
    uint64_t Sz; //must be 0
    uint64_t base; // Since 4KB-aligned, 12 bits are useless, 52(total) - 12 = 40 bits left
    // will ignore the low 12 bits as well as the high 12 bits of this field
    uint64_t XD;
} pml4_entry_t, pdpt_entry_t, pd_entry_t;

typedef struct __attribute__ ((packed))
{
    uint64_t Pr;
    uint64_t RW;
    uint64_t USU;
    uint64_t PWT;
    uint64_t PCD;
    uint64_t Acc;
    uint64_t dirty;
    uint64_t PAT;
    uint64_t Gl;
    uint64_t base; // Since 4KB-aligned, 12 bits are useless, 52(total) - 12 = 40 bits left
    // will ignore the low 12 bits as well as the high 12 bits of this field
    uint64_t XD;
} pt_entry_t;

typedef struct __attribute__ ((packed))
{
    uint64_t base;
    uint64_t limit;
    uint64_t type;
    uint64_t DPL;
    uint64_t Gr;
    uint64_t Acc;
    uint64_t Pr;
    uint64_t Sz; //32 bits = 1, 16 bits = 0
    uint64_t x64;
    uint64_t Sys; //System = 0, code/data = 1
} segment_descriptor_t;

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

void HYPKERNEL64 hk_write_segment_descriptor(uint8_t *const gdt, segment_descriptor_t const *const seg_desc);

//extern void HYPKERNEL64 hk_load_gdt(gdt_ptr_t const *const ptr, uint16_t const sel_code, uint16_t const sel_data);

void HYPKERNEL64 hk_mem_cpy(void *src, void *dst, uint64_t size);

void HYPKERNEL64 hk_mem_move(void *src, void *dst, uint64_t size);

void HYPKERNEL64 hk_mem_set(void *src, int8_t const val, uint64_t size);

void HYPKERNEL64 hk_write_pt_entry(uint8_t *const base, pt_entry_t const *const p_entry);

void HYPKERNEL64 hk_write_pd_entry(uint8_t *const base, pd_entry_t const *const p_entry);

void HYPKERNEL64 hk_write_pdpt_entry(uint8_t *const base, pdpt_entry_t const *const p_entry);

void HYPKERNEL64 hk_write_pml4_entry(uint8_t *const base, pml4_entry_t const *const p_entry);

#endif
