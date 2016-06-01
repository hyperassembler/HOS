#include "k_intr.h"
#include "k_hal.h"

void k_set_interrupt_handler(uint64_t index, void (*handler)(void))
{
    hal_set_interrupt_handler(index, handler);
}

void k_disable_interrupt()
{

}

void k_enable_interrupt()
{
    
}