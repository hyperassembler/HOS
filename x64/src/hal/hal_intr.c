/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "bit_ops.h"
#include "hal_arch.h"
#include "hal_intr.h"
#include "hal_print.h"
#include "hal_mem.h"
#include "hal_var.h"

void KAPI hal_write_gate(void *const gate,
                                uint64_t const offset,
                                uint32_t const selector,
                                uint32_t const attr)
{
    ((uint8_t *) gate)[0] = (uint8_t) (offset & 0xFF);
    ((uint8_t *) gate)[1] = (uint8_t) ((offset >> 8) & 0xFF);
    ((uint8_t *) gate)[2] = (uint8_t) (selector & 0xFF);
    ((uint8_t *) gate)[3] = (uint8_t) ((selector >> 8) & 0xFF);
    ((uint8_t *) gate)[4] = (uint8_t) (attr & 0xFF);
    ((uint8_t *) gate)[5] = (uint8_t) ((attr >> 8) & 0xFF);
    ((uint8_t *) gate)[6] = (uint8_t) ((offset >> 16) & 0xFF);
    ((uint8_t *) gate)[7] = (uint8_t) ((offset >> 24) & 0xFF);
    ((uint8_t *) gate)[8] = (uint8_t) ((offset >> 32) & 0xFF);
    ((uint8_t *) gate)[9] = (uint8_t) ((offset >> 40) & 0xFF);
    ((uint8_t *) gate)[10] = (uint8_t) ((offset >> 48) & 0xFF);
    ((uint8_t *) gate)[11] = (uint8_t) ((offset >> 56) & 0xFF);
    ((uint8_t *) gate)[12] = 0;
    ((uint8_t *) gate)[13] = 0;
    ((uint8_t *) gate)[14] = 0;
    ((uint8_t *) gate)[15] = 0;
    return;
}

void KAPI hal_set_interrupt_handler(uint64_t index,
                                           void (*handler)(void))
{
    if(index < IDT_ENTRY_NUM)
    {
        hal_write_gate(g_idt + 16 * index, (uint64_t) handler, seg_selector(1, 0),
                       GATE_DPL_0 | GATE_PRESENT | GATE_TYPE_INTERRUPT);
    }
    return;
}

void KAPI hal_register_interrupt_handler(uint64_t index,
                                         void (*handler)(uint64_t pc,
                                                         uint64_t sp,
                                                         uint64_t error))
{
    if(index < IDT_ENTRY_NUM)
    {
        g_intr_handler_table[index] = handler;
    }
    return;
}

void KAPI hal_deregister_interrupt_handler(uint64_t index)
{
    if(index < IDT_ENTRY_NUM)
    {
        g_intr_handler_table[index] = NULL;
    }
    return;
}

void KAPI hal_assert(int64_t expression,
                            char *message)
{
    if (!expression)
    {
        hal_printf("HAL: Assertion failed. Detail: %s", message == NULL ? "NULL" : message);
        hal_halt_cpu();
    }
    return;
}

void KAPI hal_interrupt_dispatcher(uint64_t int_vec, hal_intr_context_t* context, uint64_t error_code)
{
    if(g_intr_handler_table[int_vec] == NULL)
    {
        hal_printf("Unhandled interrupt %d at 0x%X. Err: %d.\n", int_vec, context->rip, error_code);
    }
    else
    {
        g_intr_handler_table[int_vec](context->rip, context->rsp, int_vec);
    }
    return;
}

static void KAPI _hal_populate_idt()
{
    hal_set_interrupt_handler(0, hal_interrupt_handler_0);
    hal_set_interrupt_handler(1, hal_interrupt_handler_1);
    hal_set_interrupt_handler(2, hal_interrupt_handler_2);
    hal_set_interrupt_handler(3, hal_interrupt_handler_3);
    hal_set_interrupt_handler(4, hal_interrupt_handler_4);
    hal_set_interrupt_handler(5, hal_interrupt_handler_5);
    hal_set_interrupt_handler(6, hal_interrupt_handler_6);
    hal_set_interrupt_handler(7, hal_interrupt_handler_7);
    hal_set_interrupt_handler(8, hal_interrupt_handler_8);
    hal_set_interrupt_handler(10, hal_interrupt_handler_10);
    hal_set_interrupt_handler(11, hal_interrupt_handler_11);
    hal_set_interrupt_handler(12, hal_interrupt_handler_12);
    hal_set_interrupt_handler(13, hal_interrupt_handler_13);
    hal_set_interrupt_handler(14, hal_interrupt_handler_14);
    hal_set_interrupt_handler(16, hal_interrupt_handler_16);
    hal_set_interrupt_handler(17, hal_interrupt_handler_17);
    hal_set_interrupt_handler(18, hal_interrupt_handler_18);
    hal_set_interrupt_handler(19, hal_interrupt_handler_19);
    hal_set_interrupt_handler(20, hal_interrupt_handler_20);

    hal_set_interrupt_handler(32, hal_interrupt_handler_32);
    hal_set_interrupt_handler(33, hal_interrupt_handler_33);
    hal_set_interrupt_handler(34, hal_interrupt_handler_34);
    hal_set_interrupt_handler(35, hal_interrupt_handler_35);
    hal_set_interrupt_handler(36, hal_interrupt_handler_36);
    hal_set_interrupt_handler(37, hal_interrupt_handler_37);
    hal_set_interrupt_handler(38, hal_interrupt_handler_38);
    hal_set_interrupt_handler(39, hal_interrupt_handler_39);

    return;
}

int32_t KAPI hal_interrupt_init(void)
{
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    eax = 1;
    hal_cpuid(&eax,&ebx,&ecx,&edx);
    if(!(edx & bit_mask_32(9)))
    {
        hal_printf("ERROR: APIC not supported by CPU.\n");
        return 1;
    }

    // get idt ptr ready
    g_idt_ptr.base = (uint64_t) g_idt;
    g_idt_ptr.limit = IDT_ENTRY_NUM * IDT_ENTRY_SIZE - 1;

    // clear dispatch table
    for(uint64_t i = 0; i < IDT_ENTRY_NUM; i++)
    {
        g_intr_handler_table[i] = NULL;
    }

    // hook asm interrupt handlers
    _hal_populate_idt();

    hal_flush_idt(&g_idt_ptr);

    // disable PIC
    hal_write_port(0xa1, 0xff);
    hal_write_port(0x21, 0xff);

    uint64_t apic_base_reg = 0;
    uint64_t apic_base = 0;
    ecx = MSR_IA32_APIC_BASE;
    hal_read_msr(&ecx, &edx, &eax);
    apic_base_reg = ((uint64_t)edx << 32) + (uint64_t)eax;
    apic_base = apic_base_reg & bit_field_mask_64(12,35);
    hal_printf("APIC Base: 0x%X\n", apic_base);
    hal_printf("APIC Enabled: %s\n", apic_base_reg & bit_mask_64(11) ? "Yes" : "No");
    hal_printf("BSP: %s\n", apic_base_reg & bit_mask_64(8) ? "Yes" : "No");
    hal_printf("APIC Spour: 0x%X\n", *(uint32_t*)((char*)apic_base + APIC_SPURIOUS_INT_VEC_REG_OFFSET));
    // hardware enable APIC
    ecx = MSR_IA32_APIC_BASE;
    eax = (uint32_t)(apic_base_reg & bit_field_mask_64(0,31)) | bit_mask_32(11);
    hal_write_msr(&ecx, &edx, &eax);

    // software enable APIC
    hal_write_mem_32((char*)apic_base + APIC_SPURIOUS_INT_VEC_REG_OFFSET,
                     *(uint32_t*)((char*)apic_base + APIC_SPURIOUS_INT_VEC_REG_OFFSET) | bit_mask_32(8));

//    for(int i = 0; i < IDT_ENTRY_NUM; i++)
//    {
//        hal_printf("IDT%d: %d,%d,%d,%d",i, (uint64_t)g_idt);
//    }
    hal_trigger_interrupt(33);
    hal_halt_cpu();
    hal_enable_interrupt();

    return 0;
}