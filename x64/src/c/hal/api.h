#ifndef HAL_API_H
#define HAL_API_H

#include "../common/kdef.h"
#include "../common/type.h"
//INTERRUPT
void NATIVE64 hal_set_interrupt_handler(uint64_t index, void (*handler)(void));
void NATIVE64 hal_enable_interrupt();
void NATIVE64 hal_disable_interrupt();

//concurrency
extern void NATIVE64 hal_spin_lock(uint32_t * lock);
extern void NATIVE64 hal_spin_unlock(uint32_t * lock);

//output
void NATIVE64 hal_printf();

#endif
