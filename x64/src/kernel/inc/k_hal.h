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
extern void KAPI hal_register_interrupt_handler(uint64_t index,
                                                void (*handler)(uint64_t pc,
                                                                uint64_t sp,
                                                                uint64_t error));

extern void KAPI hal_deregister_interrupt_handler(uint64_t index);

#define hal_trigger_interrupt(x) __asm__ __volatile__ ("int "#x);

// concurrency
extern uint64_t KAPI hal_interlocked_exchange(uint64_t* dst, uint64_t val);

// loaded kernel addr
extern char kernel_start[];
extern char kernel_end[];

// hal init
extern k_hal_info_t* KAPI hal_init(char* m_info);

// hal output
extern void KAPI hal_printf(char const *format, ...);
extern void KAPI hal_clear_screen(void);

// hal CPU control
extern void KAPI hal_halt_cpu(void);

// hal memory

//debug
extern void KAPI hal_bochs_magic_breakpoint();

#endif
