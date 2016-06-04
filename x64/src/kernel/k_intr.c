#include "k_intr.h"
#include "k_hal.h"

void KAPI k_set_interrupt_handler(uint64_t index, void (*handler)(void))
{
    hal_set_interrupt_handler(index, handler);
}

void KAPI k_disable_interrupt()
{
    hal_disable_interrupt();
}

void KAPI k_enable_interrupt()
{
    hal_disable_interrupt();
}