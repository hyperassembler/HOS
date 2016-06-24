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

extern uint8_t g_gdt[GDT_ENTRY_NUM*GDT_ENTRY_SIZE];
extern uint8_t g_idt[IDT_ENTRY_NUM*IDT_ENTRY_SIZE];
extern k_intr_handler_t g_intr_handler_table[IDT_ENTRY_NUM];
extern void* g_intr_handler_context_table[IDT_ENTRY_NUM];
extern k_exc_handler_t g_exc_handler_table[IDT_ENTRY_NUM];
extern hal_gdt_ptr_t g_gdt_ptr;
extern hal_idt_ptr_t g_idt_ptr;
extern uint64_t text_pos;
extern char kernel_start[];
extern char kernel_end[];

#endif