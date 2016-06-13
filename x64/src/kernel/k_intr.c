#include "k_intr.h"
#include "k_hal.h"

void KAPI k_register_interrupt_handler(uint64_t index, void (*handler)(uint64_t pc, uint64_t sp, uint64_t error))
{
    hal_register_interrupt_handler(index, handler);
}

void KAPI k_deregister_interrupt_handler(uint64_t index, void (*handler)(uint64_t pc, uint64_t sp, uint64_t error))
{
    hal_register_interrupt_handler(index, handler);
}

void KAPI k_disable_interrupt()
{
    hal_disable_interrupt();
}

void KAPI k_enable_interrupt()
{
    hal_disable_interrupt();
}