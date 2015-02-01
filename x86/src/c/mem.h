#ifndef _MEM_32_H_
#define _MEM_32_H_
#include "type.h"
#include "kdef.h"

typedef struct __attribute__ ((packed))
{
    uint8_t Pr;
    uint8_t RW;
    uint8_t USU;
    uint8_t PWT;
    uint8_t PCD;
    uint8_t Acc;
    uint8_t Sz; //must be 0
    uint64_t base; // Since 4KB-aligned, 12 bits are useless, 52(total) - 12 = 40 bits left
                 // will ignore the low 12 bits as well as the high 12 bits of this field
                 uint8_t XD;
} pml4_entry_t, pdpt_entry_t, pd_entry_t;

typedef struct __attribute__ ((packed))
{
    uint8_t Pr;
    uint8_t RW;
    uint8_t USU;
    uint8_t PWT;
    uint8_t PCD;
    uint8_t Acc;
    uint8_t dirty;
    uint8_t PAT;
    uint8_t Gl;
    uint64_t base; // Since 4KB-aligned, 12 bits are useless, 52(total) - 12 = 40 bits left
    // will ignore the low 12 bits as well as the high 12 bits of this field
    uint8_t XD;
} pt_entry_t;

typedef struct __attribute__ ((packed))
{
    uint32_t offset;
    uint16_t seg_sel;
    uint8_t Pr;
    uint8_t DPL;
    uint8_t Sz;
} interrupt_gate_t;

typedef struct __attribute__ ((packed))
{
    uint32_t offset;
    uint16_t seg_sel;
    uint8_t Pr;
    uint8_t DPL;
    uint8_t Sz;
} trap_gate_t;

typedef struct __attribute__ ((packed))
{
    uint16_t tss_sel;
    uint8_t DPL;
    uint8_t Pr;
} task_gate_t;

typedef struct __attribute__ ((packed))
{
    uint32_t base;
    uint32_t limit;
    uint8_t type;
    uint8_t DPL;
    uint8_t Gr;
    uint8_t Acc;
    uint8_t Pr;
    uint8_t Sz; //32 bits = 1, 16 bits = 0
    uint8_t x64;
    uint8_t Sys; //System = 0, code/data = 1

} segment_descriptor_t;

typedef struct __attribute__ ((packed))
{
	uint16_t limit;
	uint32_t base;
} gdt_ptr_t;

typedef struct __attribute__ ((packed))
{
    uint16_t limit;
    uint64_t base;
} gdt_ptr_64_t;

typedef struct __attribute__ ((packed))
{
    uint16_t limit;
    uint32_t base;
} idt_ptr_t;

void HYPKERNEL32 hk_write_segment_descriptor(uint8_t *const gdt, segment_descriptor_t const *const seg_desc);
extern void HYPKERNEL32 hk_load_gdt(gdt_ptr_t const * const ptr, uint16_t const sel_code, uint16_t const sel_data);
void HYPKERNEL32 hk_write_interrupt_gate(uint8_t *const dst, interrupt_gate_t const *int_gate);
void HYPKERNEL32 hk_write_trap_gate(uint8_t *const dst, trap_gate_t const *tr_gate);
void HYPKERNEL32 hk_write_task_gate(uint8_t *const dst, task_gate_t const *int_gate);
void HYPKERNEL32 hk_mem_cpy(void* src, void* dst, uint32_t size);
void HYPKERNEL32 hk_mem_move(void* src, void* dst, uint32_t size);
extern int32_t HYPKERNEL32 hk_support_x64(void);
extern void hk_disable_paging(void);
extern void hk_enable_paging(void);
void HYPKERNEL32 hk_mem_set(void* src, int8_t const val,uint32_t size);
void HYPKERNEL32 hk_write_pt_entry(uint8_t * const base, pt_entry_t const * const p_entry);
void HYPKERNEL32 hk_write_pd_entry(uint8_t * const base, pd_entry_t const * const p_entry);
void HYPKERNEL32 hk_write_pdpt_entry(uint8_t * const base, pdpt_entry_t const * const p_entry);
void HYPKERNEL32 hk_write_pml4_entry(uint8_t * const base, pml4_entry_t const * const p_entry);
#endif
