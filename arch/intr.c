#include <arch/intr.h>
#include <arch/mem.h>
#include <arch/print.h>
#include <kern/print.h>
#include <kern/status.h>
#include <kern/brute.h>

#include "cpu.h"

#define HAL_CORE_COUNT 1

static uint8 cpu_idts[HAL_CORE_COUNT][IDT_ENTRY_NUM * IDT_ENTRY_SIZE];
static struct hal_idt_ptr cpu_idt_ptrs[HAL_CORE_COUNT];

static void *k_intr_disps[HAL_CORE_COUNT];

int
arch_raise_irql(ATTR_UNUSED int irql)
{
    return 0;
}

int
arch_lower_irql(ATTR_UNUSED int irql)
{
    return 0;
}

int
arch_get_irql()
{
    return 0;
}

void
hal_write_gate(void *gate, const uint64 offset, const uint32 selector, uint32 const attr)
{
    ((uint8 *) gate)[0] = (uint8) (offset & 0xFFu);
    ((uint8 *) gate)[1] = (uint8) ((offset >> 8u) & 0xFFu);
    ((uint8 *) gate)[2] = (uint8) (selector & 0xFFu);
    ((uint8 *) gate)[3] = (uint8) ((selector >> 8u) & 0xFFu);
    ((uint8 *) gate)[4] = (uint8) (attr & 0xFFu);
    ((uint8 *) gate)[5] = (uint8) ((attr >> 8u) & 0xFFu);
    ((uint8 *) gate)[6] = (uint8) ((offset >> 16u) & 0xFFu);
    ((uint8 *) gate)[7] = (uint8) ((offset >> 24u) & 0xFFu);
    ((uint8 *) gate)[8] = (uint8) ((offset >> 32u) & 0xFFu);
    ((uint8 *) gate)[9] = (uint8) ((offset >> 40u) & 0xFFu);
    ((uint8 *) gate)[10] = (uint8) ((offset >> 48u) & 0xFFu);
    ((uint8 *) gate)[11] = (uint8) ((offset >> 56u) & 0xFFu);
    ((uint8 *) gate)[12] = 0;
    ((uint8 *) gate)[13] = 0;
    ((uint8 *) gate)[14] = 0;
    ((uint8 *) gate)[15] = 0;
}

void KABI
hal_interrupt_dispatcher(uint64 int_vec, struct interrupt_context *context)
{
    uint32 coreid = 0;
    if (k_intr_disps[coreid] == NULL) {
        kprintf("Unhandled interrupt %ld at 0x%lx.\n", int_vec, context->rip);
    }
    /*else
    {
        k_intr_disps[coreid]((uint32) int_vec, context);
    }*/
}

static void
populate_idt(void)
{

}

void
arch_intr_init(void)
{
    uint32 coreid;
    uint32 eax;
    uint32 ebx;
    uint32 ecx;
    uint32 edx;

    // detect APIC first
    eax = 1;
    arch_cpuid(&eax, &ebx, &ecx, &edx);
    if (!(edx & (1u << 9u))) {
        BRUTE("ERROR: APIC is not present.\n");
    }

    coreid = 0;

    // get idt ptr ready
    cpu_idt_ptrs[coreid].base = (uint64) &cpu_idts[coreid];
    cpu_idt_ptrs[coreid].limit = IDT_ENTRY_NUM * IDT_ENTRY_SIZE - 1;

    // clear dispatch table
    k_intr_disps[coreid] = NULL;

    // hook asm interrupt handlers
    populate_idt();

    arch_flush_idt(&cpu_idt_ptrs[coreid]);

    // disable PIC
    arch_write_port_8(0xa1, 0xff);
    arch_write_port_8(0x21, 0xff);

    uint64 apic_base_reg = 0;
    ATTR_UNUSED uint64 apic_base = 0;
    ecx = 0;
    arch_read_msr(&ecx, &edx, &eax);
    apic_base_reg = ((uint64) edx << 32u) + (uint64) eax;
    //apic_base = apic_base_reg & bit_field_mask(12, 35);


    // hardware enable APIC
    ecx = 0;
    // eax = (uint32) ((apic_base_reg & bit_field_mask(0, 31)) | (1 << 11));
    arch_write_msr(&ecx, &edx, &eax);
}
