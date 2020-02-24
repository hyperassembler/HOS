#pragma once

#include <common/cdef.h>

#define HAL_CORE_COUNT 1
struct ATTR_PACKED hal_gdt_ptr {
    uint16 limit;
    uint64 base;
};

struct ATTR_PACKED hal_idt_ptr {
    uint16 limit;
    uint64 base;
};

void KABI out_8(uint16 port, uint8 data);

void KABI out_16(uint16 port, uint16 data);

void KABI out_32(uint16 port, uint32 data);

uint8 KABI in_8(uint16 port);

uint16 KABI in_16(uint16 port);

uint32 KABI in_32(uint16 port);

void KABI flush_gdt(void *gdt_ptr, uint16 code_slct, uint16 data_slct);

void KABI flush_idt(void *idt_ptr);

void KABI flush_tss(uint16 tss_slct);

void KABI cpuid(uint32 *eax, uint32 *ebx, uint32 *ecx, uint32 *edx);

void KABI read_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

void KABI write_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

void KABI sti();

void KABI cli();

uint64 KABI read_cr8();

void KABI write_cr8(uint64 val);

uint64 KABI read_cr3();

void KABI write_cr3(uint64 val);

void KABI flush_tlb();


/**
 * ASM declaration
 */
void KABI arch_cpuid(uint32 *eax, uint32 *ebx, uint32 *ecx, uint32 *edx);

void KABI arch_halt(void);

void KABI arch_flush_gdt(struct hal_gdt_ptr *gdt_ptr, uint64 code_slct, uint64 data_slct);

void KABI arch_flush_tlb(void);

void KABI arch_flush_idt(struct hal_idt_ptr *idt_ptr);

void KABI arch_read_idt(struct hal_idt_ptr **idt_ptr);

void KABI arch_read_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

void KABI arch_write_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

void KABI arch_write_cr3(uint64 base);

uint64 KABI arch_read_cr3(void);

void KABI arch_write_cr8(uint64 pri);

uint64 KABI arch_read_cr8(void);

int8 KABI arch_read_port_8(uint16 port);

int16 KABI arch_read_port_16(uint16 port);

int32 KABI arch_read_port_32(uint16 port);

void KABI arch_write_port_8(uint16 port, uint8 data);

void KABI arch_write_port_16(uint16 port, uint16 data);

void KABI arch_write_port_32(uint16 port, uint32 data);
