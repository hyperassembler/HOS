/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "hal_intr.h"
#include "hal_print.h"
#include "hal_mem.h"

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
    hal_write_gate(g_idt + 16 * index, (uint64_t) handler, seg_selector(1, 0),
                   GATE_DPL_0 | GATE_PRESENT | GATE_TYPE_INTERRUPT);
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
