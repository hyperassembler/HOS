#include "common.h"

#define HABI KABI

/**
 * ASM Functions
 */


/**
 * CPU Instructions
 */
void HABI hal_cpuid(uint32 *eax, uint32 *ebx, uint32 *ecx, uint32 *edx);

void HABI hal_halt_cpu(void);

void HABI hal_enable_interrupt(void);

void HABI hal_disable_interrupt(void);

/**
 * IO Port Operations
 */

int8 HABI hal_read_port_8(uint16 port);

int16 HABI hal_read_port_16(uint16 port);

int32 HABI hal_read_port_32(uint16 port);

void HABI hal_write_port_8(uint16 port, uint8 data);

void HABI hal_write_port_16(uint16 port, uint16 data);

void HABI hal_write_port_32(uint16 port, uint32 data);


/**
 * CPU Structure Operations
 */
void HABI hal_flush_gdt(struct hal_gdt_ptr *gdt_ptr, uint64 code_slct, uint64 data_slct);

void HABI hal_flush_tlb(void);

void HABI hal_flush_idt(struct hal_idt_ptr *idt_ptr);

void HABI hal_read_idt(struct hal_idt_ptr **idt_ptr);

/**
 * Control Register Operations
 */
#define MSR_IA32_APIC_BASE 0x1B

void HABI hal_read_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

void HABI hal_write_msr(uint32 *ecx, uint32 *edx, uint32 *eax);

void HABI hal_write_cr3(uint64 base);

uint64 HABI hal_read_cr3(void);

void HABI hal_write_cr8(uint64 pri);

uint64 HABI hal_read_cr8(void);


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
