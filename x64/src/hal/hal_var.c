/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "hal_intr.h"
#include "hal_var.h"

uint8_t g_gdt[8*9];
uint8_t g_idt[IDT_ENTRY_NUM*IDT_ENTRY_SIZE];
void (*g_intr_handler_table[IDT_ENTRY_SIZE])(uint64_t pc, uint64_t sp, uint64_t error);
hal_gdt_ptr_t g_gdt_ptr;
hal_idt_ptr_t g_idt_ptr;
uint64_t text_pos;