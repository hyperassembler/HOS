#pragma once

#include <ke/cdef.h>

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