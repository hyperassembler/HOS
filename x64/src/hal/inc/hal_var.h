/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _HAL_VAR_H_
#define _HAL_VAR_H_

#include "hal_mem.h"
#include "linked_list.h"
#include "hal_arch.h"
#include "hal_intr.h"

extern uint8_t g_gdt[];
extern uint8_t g_idt[];
extern void (*g_intr_handler_table[IDT_ENTRY_SIZE])(uint64_t pc, uint64_t sp, uint64_t error);
extern hal_gdt_ptr_t g_gdt_ptr;
extern hal_idt_ptr_t g_idt_ptr;
extern uint64_t text_pos;

#endif