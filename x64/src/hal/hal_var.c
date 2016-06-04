/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "hal_var.h"

uint8_t g_gdt[8*9];
uint8_t g_idt[21*16];
gdt_ptr_t g_gdt_ptr;
idt_ptr_t g_idt_ptr;
uint64_t text_pos;