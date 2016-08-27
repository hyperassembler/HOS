#include "bifrost_hs_intr.h"
#include "bifrost_hs_mem.h"
#include "bifrost_hs_boot.h"
#include "bifrost_hs_context.h"
#include "bifrost_types.h"
#include "bifrost_system_constants_xtensa.h"
#include "interrupt.h"
#include "context.h"
#include "atomic.h"
#include "mem.h"

int32_t ke_hal_setup(hw_arch_bootinfo_t *bootinfo)
{
    int32_t result = 0;
    if(bootinfo == NULL)
        return -1;

    result = arch_interrupt_init(&bootinfo->int_info);

    if(result != 0)
        return result;

    result = arch_mem_init(bootinfo);

    return result;
}

void ke_set_timer_timeout(uint32_t ms)
{
    uint32_t timeout = ms * (PROC_FREQUENCY_MHZ * 1000);
    arch_set_timer_timeout(timeout);
    return;
}

void ke_trigger_intr(uint32_t core, uint32_t vec)
{
    arch_trigger_interrupt(core, vec);
}

// IRQL on Xtensa has identical mapping between kernel defined and arch specific mask

hw_irql_t ke_set_irql(hw_irql_t irql)
{
    return arch_set_irql(irql);
}

hw_irql_t ke_get_irql()
{
   return arch_get_irql();
}

void ke_context_switch(void *intr_context, void *old_context, void *new_context)
{
    return arch_context_switch((UserFrame*)intr_context, (UserFrame*)old_context, (UserFrame*)new_context);
}

void ke_create_context(void *context, void *pc, void *sp, hw_irql_t irql, void *arg)
{
    arch_create_context((UserFrame*)context, pc, sp, (uint32_t)irql, arg);
}

uint32_t ke_get_current_core()
{
    return xthal_get_prid();
}

void ke_flush_addr(void *addr, uint32_t num_of_cacheline)
{
    xthal_dcache_region_writeback_inv(addr, num_of_cacheline * HW_CACHELINE_SIZE);
}

hw_intr_handler_t ke_register_intr_handler(uint32_t vec, hw_intr_handler_t handler, void *context)
{
    return arch_register_intr_handler(vec, handler, context);
}

int32_t ke_interlocked_exchange(int32_t *addr, int32_t data)
{
    int32_t orig = *addr;
    while(arch_interlocked_compare_exchange(addr, orig, data) != orig)
    {
        orig = *addr;
    }
    return orig;
}


int32_t ke_interlocked_compare_exchange(int32_t *addr, int32_t compare, int32_t val)
{
    return arch_interlocked_compare_exchange(addr, compare, val);
}

int32_t ke_interlocked_increment(int32_t *addr, int32_t val)
{
    int32_t orig = *addr;
    while(arch_interlocked_compare_exchange(addr, orig, orig + val) != orig)
    {
        orig = *addr;
    }
    return orig;
}


void ke_register_exc_handler(hw_exc_type_t type, hw_exc_handler_t handler)
{
    switch(type)
    {
        case invalid_op_exc:
            arch_register_exc_handler(HW_EXC_ILLEGAL, handler);
        case debug_exc:
            break;
        case div_by_zero_exc:
            arch_register_exc_handler(HW_EXC_DIVIDE_BY_ZERO, handler);
        case unrecoverable_exc:
            break;
        case unsupported_thr_fatal_exc:
            break;
        case unsupported_thr_nonfatal_exc:
            break;
        case page_fault_exc:
            arch_register_exc_handler(HW_EXC_INSTR_ADDR_ERROR, handler);
            arch_register_exc_handler(HW_EXC_INSTR_DATA_ERROR, handler);
            arch_register_exc_handler(HW_EXC_LOAD_STORE_ADDR_ERROR, handler);
            arch_register_exc_handler(HW_EXC_LOAD_STORE_DATA_ERROR, handler);
            break;
        case general_protection_exc:
            break;
    }
}
