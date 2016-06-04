/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _HAL_VAR_H_
#define _HAL_VAR_H_

#include "hal_mem.h"
#include "linked_list.h"

extern uint8_t g_gdt[8*9];
extern uint8_t g_idt[21*16];
extern gdt_ptr_t g_gdt_ptr;
extern idt_ptr_t g_idt_ptr;
extern uint64_t text_pos;

#endif