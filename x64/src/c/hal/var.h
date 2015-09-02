#ifndef _VAR_H_
#define _VAR_H_

#include "mem.h"
#include "../common/util/list/linked_list/linked_list.h"

extern uint8_t g_gdt[8*9];
extern uint8_t g_idt[21*16];
extern gdt_ptr_t g_gdt_ptr;
extern idt_ptr_t g_idt_ptr;
extern uint64_t text_pos;

#endif