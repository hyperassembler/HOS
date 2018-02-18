#ifndef _HAL_CPU_H_
#define _HAL_CPU_H_

#include "type.h"
#include "kernel/hal/atomic.h"

#define HAL_CORE_COUNT 1


typedef struct
{
	uint16_t limit;
	uint64_t base;
} STRUCT_PACKED hal_gdt_ptr_t;

typedef struct
{
	uint16_t limit;
	uint64_t base;
} STRUCT_PACKED hal_idt_ptr_t;


/**
 * CPU Instructions
 */
extern void KABI hal_cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

extern void KABI hal_halt_cpu(void);

extern void KABI hal_enable_interrupt(void);

extern void KABI hal_disable_interrupt(void);


/**
 * IO Port Operations
 */

extern int8_t KABI hal_read_port_8(uint16_t port);

extern int16_t KABI hal_read_port_16(uint16_t port);

extern int32_t KABI hal_read_port_32(uint16_t port);

extern void KABI hal_write_port_8(uint16_t port, uint8_t data);

extern void KABI hal_write_port_16(uint16_t port, uint16_t data);

extern void KABI hal_write_port_32(uint16_t port, uint32_t data);


/**
 * CPU Structure Operations
 */

extern void KABI hal_flush_gdt(hal_gdt_ptr_t *gdt_ptr, uint64_t code_slct, uint64_t data_slct);

extern void KABI hal_flush_tlb(void);

extern void KABI hal_flush_idt(hal_idt_ptr_t *idt_ptr);

extern void KABI hal_read_idt(hal_idt_ptr_t **idt_ptr);

/**
 * Control Register Operations
 */
#define MSR_IA32_APIC_BASE 0x1B

extern void KABI hal_read_msr(uint32_t *ecx, uint32_t *edx, uint32_t *eax);

extern void KABI hal_write_msr(uint32_t *ecx, uint32_t *edx, uint32_t *eax);

extern void KABI hal_write_cr3(uint64_t base);

extern uint64_t KABI hal_read_cr3(void);

extern void KABI hal_write_cr8(uint64_t pri);

extern uint64_t KABI hal_read_cr8(void);


#endif