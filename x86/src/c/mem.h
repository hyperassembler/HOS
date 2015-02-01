#ifndef _MEM_32_H_
#define _MEM_32_H_
#include "type.h"
#include "kdef.h"

typedef struct __attribute__ ((packed))
{
    uint8 Pr;
    uint8 RW;
    uint8 USU;
    uint8 PWT;
    uint8 PCD;
    uint8 Acc;
    uint8 Sz; //must be 0
    uint64 base; // Since 4KB-aligned, 12 bits are useless, 52(total) - 12 = 40 bits left
                 // will ignore the low 12 bits as well as the high 12 bits of this field
    uint8 XD;
} pml4_entry, pdpt_entry, pd_entry;

typedef struct __attribute__ ((packed))
{
    uint8 Pr;
    uint8 RW;
    uint8 USU;
    uint8 PWT;
    uint8 PCD;
    uint8 Acc;
    uint8 dirty;
    uint8 PAT;
    uint8 Gl;
    uint64 base; // Since 4KB-aligned, 12 bits are useless, 52(total) - 12 = 40 bits left
    // will ignore the low 12 bits as well as the high 12 bits of this field
    uint8 XD;
} pt_entry;

typedef struct __attribute__ ((packed))
{
    uint32 offset;
    uint16 seg_sel;
    uint8 Pr;
    uint8 DPL;
    uint8 Sz;
} interrupt_gate;

typedef struct __attribute__ ((packed))
{
    uint32 offset;
    uint16 seg_sel;
    uint8 Pr;
    uint8 DPL;
    uint8 Sz;
} trap_gate;

typedef struct __attribute__ ((packed))
{
    uint16 tss_sel;
    uint8 DPL;
    uint8 Pr;
} task_gate;

typedef struct __attribute__ ((packed))
{
    uint32 base;
    uint32 limit;
    uint8 type;
    uint8 DPL;
    uint8 Gr;
    uint8 Acc;
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

typedef struct __attribute__ ((packed))
{
    uint16 limit;
    uint32 base;
} idt_ptr;

void HYPKERNEL32 hk_write_segment_descriptor(uint8 *const gdt, segment_descriptor const *const seg_desc);
extern void HYPKERNEL32 hk_load_gdt(gdt_ptr const * const ptr, uint16 const sel_code, uint16 const sel_data);
void HYPKERNEL32 hk_write_interrupt_gate(uint8 *const dst, interrupt_gate const *int_gate);
void HYPKERNEL32 hk_write_trap_gate(uint8 *const dst, trap_gate const *tr_gate);
void HYPKERNEL32 hk_write_task_gate(uint8 *const dst, task_gate const *int_gate);
void HYPKERNEL32 hk_mem_cpy(void* src, void* dst, uint32 size);
void HYPKERNEL32 hk_mem_move(void* src, void* dst, uint32 size);
extern int32 HYPKERNEL32 hk_support_x64(void);
void HYPKERNEL32 hk_write_pt_entry(uint8 * const base, pt_entry const * const p_entry);
void HYPKERNEL32 hk_write_pd_entry(uint8 * const base, pd_entry const * const p_entry);
void HYPKERNEL32 hk_write_pdpt_entry(uint8 * const base, pdpt_entry const * const p_entry);
void HYPKERNEL32 hk_write_pml4_entry(uint8 * const base, pml4_entry const * const p_entry);
#endif
