/*-------------------------------------------------------
 |
 |    interrupt.c
 |
 |    Fucntions for interrupt handling on 'xtensa' architecture.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include <stdint.h>
#include "hup_chip.h"
#include "interrupt.h"
#include "xtensa/xtruntime.h"
#include "bifrost_system_constants_xtensa.h"
//#include "intc.h"

//
// Interrupt handlers
//

static void *_intr_handler_context_table[HW_PROC_CNT][HW_INT_CNT];
static hw_intr_handler_t _intr_handler_table[HW_PROC_CNT][HW_INT_CNT];
static hw_exc_handler_t _exc_handler_table[HW_PROC_CNT][HW_EXC_CNT];
static uint32_t _timer_timeout;


// Assumes: IRQL == the IRQL the dispatcher


void arch_set_timer_timeout(uint32_t timeout)
{
    _timer_timeout = timeout;
    xthal_set_ccompare(TIMER_INTR_VEC, xthal_get_ccount() + _timer_timeout);
    return;
}

void arch_intr_dispatch(uint32_t vec, UserFrame *intr_context)
{
    uint32_t coreid = xthal_get_prid();
    // save user frame
    if (vec >= HW_INT_CNT)
    {
        // TODO PRINT INVALID VECTOR
    }

    if (vec == TIMER_INTR_VEC)
    {
        xthal_set_ccompare(TIMER_INTR_VEC, xthal_get_ccount() + _timer_timeout);
    }

    if (_intr_handler_table[coreid][vec] == NULL)
    {
        // TODO PRINT UNHANDLED INTERRUPT
    }
    else
    {
        _intr_handler_table[coreid][vec](intr_context, _intr_handler_context_table[vec]);
    }

    return;
}

void arch_trigger_interrupt(uint32_t core, uint32_t vec)
{
    if (core == xthal_get_prid())
    {
        xthal_set_intset(1u << vec);
    }
    else
    {
        // TODO:
        // lib_intc::IntCtrl_SetInt(core, vec);
        HUP_chip *chip = (HUP_chip * )(0x0);
        chip->intc.proc_ints_t[ke_get_current_core()].proc_ints._int[3].interrupt_data.data = vec;
    }
}

void arch_inter_proc_intr_handler(void *context, void *ignored)
{
    uint32_t vec;
    HUP_chip *chip = (HUP_chip * )(0x0);

    vec = chip->intc.proc_ints_t[ke_get_current_core()].proc_ints._int[3].interrupt_data.data;
    chip->intc.proc_ints_t[ke_get_current_core()].proc_ints._int[3].interrupt_data.data = 0;
    xthal_set_intset(1u << vec);
    return;
}

void arch_exc_dispatch(uint32_t vec, UserFrame *context)
{
    uint32_t coreid = xthal_get_prid();

    if (vec >= HW_EXC_CNT)
    {
        // TODO PRINT INVALID VECTOR
    }

    if (_exc_handler_table[coreid][vec] == NULL)
    {
        // TODO PRINT UNHANDLED EXCEPTION
    }
    else
    {
        _exc_handler_table[coreid][vec]((uint64_t) context->pc, (uint64_t) context->a1, (uint64_t) context->exccause);
    }

    return;
}

int32_t arch_interrupt_init(hw_arch_intr_info_t *intr_info)
{
    if (intr_info == NULL)
        return 1;

    HUP_chip *chip = (HUP_chip * )(0x0);
    
    arch_set_irql(HW_IRQL_DISABLED_LEVEL);

    // Enable dispatch
    _xtos_dispatch_level1_interrupts();
    _xtos_dispatch_level2_interrupts();
    _xtos_dispatch_level3_interrupts();

    uint32_t coreid = xthal_get_prid();
    for (int j = 0; j < HW_INT_CNT; j++)
    {
        _intr_handler_context_table[coreid][j] = NULL;
        _intr_handler_table[coreid][j] = NULL;
    }

    for (int i = 0; i < HW_EXC_CNT; i++)
    {
        if (i != XCHAL_EXCCAUSE_SYSTEM_CALL &&
            i != XCHAL_EXCCAUSE_LEVEL1_INTERRUPT &&
            i != XCHAL_EXCCAUSE_ALLOCA)
        {
            _xtos_set_exception_handler(i, (_xtos_handler) arch_exc_dispatch);
        }
    }

    for (int j = 0; j < HW_EXC_CNT; j++)
    {
        _exc_handler_table[coreid][j] = NULL;
    }

    for (int i = 0; i < HW_INT_CNT; i++)
    {
        _xtos_set_interrupt_handler(i, (_xtos_handler) arch_intr_dispatch);
    }

    // Enable IPI int vector at INTC
    chip->intc.proc_ints_t[ke_get_current_core()].proc_ints.interrupt_line_mask.AsUINT32 = 1 << 3;
    
    arch_register_intr_handler(HW_INT_INTC_3, arch_inter_proc_intr_handler, NULL);
    // enable all interrupts but set IRQL = DISABLED
    xthal_int_enable(1 << TIMER_INTR_VEC);
    xthal_int_enable(1 << DPC_INTR_VEC);
    xthal_int_enable(1 << APC_INTR_VEC);
    xthal_int_enable(1 << HW_INT_INTC_3);

    intr_info->timer_vec = TIMER_INTR_VEC;
    intr_info->apc_vec = APC_INTR_VEC;
    intr_info->dpc_vec = DPC_INTR_VEC;
    xthal_set_ccompare(TIMER_INTR_VEC, 0xFFFFFFFF);

    return 0;
}


hw_intr_handler_t arch_register_intr_handler(uint32_t vec, hw_intr_handler_t handler, void *context)
{
    if (vec >= HW_INT_CNT)
        return NULL;

    uint32_t core = xthal_get_prid();

    hw_intr_handler_t old = _intr_handler_table[core][vec];
    _intr_handler_table[core][vec] = handler;
    _intr_handler_context_table[core][vec] = context;

    return old;
}

void arch_register_exc_handler(uint32_t vec, hw_exc_handler_t handler)
{
    if (vec >= HW_INT_CNT)
        return;
    uint32_t core = xthal_get_prid();
    _exc_handler_table[core][vec] = handler;
    return;
}

void arch_suspend()
{
    __asm volatile("waiti 0");
}
