/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "hal_intr.h"
#include "hal_var.h"

uint8_t g_gdt[GDT_ENTRY_NUM*GDT_ENTRY_SIZE];
uint8_t g_idt[IDT_ENTRY_NUM*IDT_ENTRY_SIZE];
k_intr_handler_t g_intr_handler_table[IDT_ENTRY_NUM];
k_exc_handler_t g_exc_handler_table[IDT_ENTRY_NUM];
hal_gdt_ptr_t g_gdt_ptr;
hal_idt_ptr_t g_idt_ptr;
uint64_t text_pos;