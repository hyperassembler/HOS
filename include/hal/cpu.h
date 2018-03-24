#ifndef HAL_CPU_H
#define HAL_CPU_H

#include "type.h"
#include "kernel/hal/atomic.h"

#define HAL_CORE_COUNT 1


struct STRUCT_PACKED hal_gdt_ptr
{
	uint16 limit;
	uint64 base;
};

struct STRUCT_PACKED hal_idt_ptr
{
	uint16 limit;
	uint64 base;
};


/**
 * CPU Instructions
 */
extern void SXAPI hal_cpuid(uint32 *eax, uint32 *ebx, uint32 *ecx, uint32 *edx);

extern void SXAPI hal_halt_cpu(void);

extern void SXAPI hal_enable_interrupt(void);

extern void SXAPI hal_disable_interrupt(void);


/**
 * IO Port Operations
 */

extern int8 SXAPI hal_read_port_8(uint16 port);

extern int16 SXAPI hal_read_port_16(uint16 port);

extern int32 SXAPI hal_read_port_32(uint16 port);

extern void SXAPI hal_write_port_8(uint16 port, uint8 data);

extern void SXAPI hal_write_port_16(uint16 port, uint16 data);

extern void SXAPI hal_write_port_32(uint16 port, uint32 data);


/**
 * CPU Structure Operations
 */

extern void SXAPI hal_flush_gdt(struct hal_gdt_ptr *gdt_ptr, uint64 code_slct, uint64 data_slct);

extern void SXAPI hal_flush_tlb(void);

extern void SXAPI hal_flush_idt(struct hal_idt_ptr *idt_ptr);

extern void SXAPI hal_read_idt(struct hal_idt_ptr **idt_ptr);

/**
 * Control Register Operations
 */
#define MSR_IA32_APIC_BASE 0x1B

extern void SXAPI hal_read_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

extern void SXAPI hal_write_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

extern void SXAPI hal_write_cr3(uint64 base);

extern uint64 SXAPI hal_read_cr3(void);

extern void SXAPI hal_write_cr8(uint64 pri);

extern uint64 SXAPI hal_read_cr8(void);


#endif
