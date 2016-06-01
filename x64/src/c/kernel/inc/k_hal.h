/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _K_HAL_H_
#define _K_HAL_H_
#include "k_def.h"
#include "k_type.h"
#include "k_sys_info.h"

// hal interrupt
extern void KAPI hal_enable_interrupt();
extern void KAPI hal_disable_interrupt();
extern void KAPI hal_mask_interrupt();
extern void KAPI hal_unmask_interrupt();
extern void KAPI hal_set_interrupt_handler(uint64_t index, void (*handler)(void));

// concurrency
extern void KAPI hal_spin_lock(uint64_t * lock);
extern void KAPI hal_spin_unlock(uint64_t * lock);

// loaded kernel addr
extern char kernel_start[];
extern char kernel_end[];

// hal init
extern boot_info_t* KAPI hal_init(void* m_info);

// hal output
extern void KAPI hal_printf(char const *format, ...);
extern void KAPI hal_clear_screen(void);

// hal CPU control
extern void KAPI hal_halt_cpu(void);

// hal memory

//debug
extern void KAPI BOCHS_MAGIC_BREAKPOINT();

#endif
