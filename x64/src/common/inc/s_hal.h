/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */


/* This file should be only be included by kernel c source files
 * and never any other files
 */

#ifndef _S_HAL_H_
#define _S_HAL_H_

#include "s_def.h"
#include "s_type.h"
#include "linked_list.h"
#include "s_intr.h"
#include "s_boot.h"

//
// HAL Initialization API
//
extern int32_t KAPI hal_init(void *multiboot_info, k_boot_info_t *boot_info);

//
// Interrupt APIs
//
extern void KAPI hal_enable_interrupt();

extern void KAPI hal_disable_interrupt();

extern void KAPI hal_set_interrupt_priority(uint64_t priority);

extern uint64_t KAPI hal_read_interrupt_priority();

extern int32_t KAPI hal_register_interrupt_handler(k_handler_type_t type,
                                                   uint64_t priority,
                                                   void (*handler)(uint64_t pc,
                                                                   uint64_t sp,
                                                                   uint64_t error));

extern void KAPI hal_deregister_interrupt_handler(int32_t index);


extern void KAPI hal_issue_interrupt(uint32_t target_core, uint32_t vector);

//
// HAL Atomic
//
extern uint64_t KAPI hal_interlocked_exchange(uint64_t *dst, uint64_t val);

//
// HAL Arch API
//
extern void KAPI hal_halt_cpu(void);

//
// HAL Print
//
extern void KAPI hal_clear_screen(void);
extern void KAPI hal_printf(char const *format, ...);

#endif
