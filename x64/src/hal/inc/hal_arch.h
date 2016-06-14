#ifndef _HAL_ARCH_H_
#define _HAL_ARCH_H_

#include "k_def.h"

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__ ((packed)) hal_gdt_ptr_t;

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__ ((packed)) hal_idt_ptr_t;

extern uint64_t KAPI hal_interlocked_exchange(uint64_t *dst, uint64_t val);

extern void KAPI hal_cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

#define MSR_IA32_APIC_BASE 0x1B
extern void KAPI hal_read_msr(uint32_t *ecx, uint32_t *edx, uint32_t *eax);

extern void KAPI hal_write_msr(uint32_t *ecx, uint32_t *edx, uint32_t *eax);

extern void KAPI hal_enable_interrupt();

extern void KAPI hal_disable_interrupt();

extern void KAPI hal_halt_cpu();

extern void KAPI hal_write_port(uint64_t port, int64_t data);

extern int64_t KAPI hal_read_port(uint64_t port);

extern void KAPI hal_write_mem_32(void* target, uint32_t data);

extern void KAPI hal_write_mem_64(void* target, uint64_t data);

extern void KAPI hal_flush_gdt(hal_gdt_ptr_t *gdt_ptr, uint64_t code_slct, uint64_t data_slct);

extern void KAPI hal_flush_tlb();

#define hal_trigger_interrupt(x) __asm__ __volatile__ ("int "#x);

extern void KAPI hal_flush_idt(hal_idt_ptr_t *idt_ptr);

extern void KAPI hal_read_idt(hal_idt_ptr_t **idt_ptr);

extern void KAPI hal_write_cr3(uint64_t base);

extern uint64_t KAPI hal_read_cr3();

#endif