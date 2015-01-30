#ifndef _MEM_32_H_
#define _MEM_32_H_
#include "type32.h"
#include "kdef32.h"

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

typedef struct __attribute__ ((packed))
{
    uint16 limit;
    uint32 base;
} idt_ptr;

int32 HYPKERNEL32 hk_set_segment_descriptor(uint8* const gdt, const segment_descriptor* const seg_desc);
extern void HYPKERNEL32 hk_load_gdt(const gdt_ptr* const ptr, const uint16 sel_code, const uint16 sel_data);
int32 HYPKERNEL32 hk_set_interrupt_gate(uint8* const dst, const interrupt_gate* int_gate);
int32 HYPKERNEL32 hk_set_trap_gate(uint8* const dst, const trap_gate* tr_gate);
int32 HYPKERNEL32 hk_set_task_gate(uint8* const dst, const task_gate* int_gate);
int32 HYPKERNEL32 hk_support_x64(void);

#endif
