#include <stdint.h>
#include "var.h"
#include "mem.h"
#include "../common/util/list/linked_list/linked_list.h"

uint8_t g_gdt[8*9];
uint8_t g_idt[21*16];
gdt_ptr_t g_gdt_ptr;
idt_ptr_t g_idt_ptr;
uint64_t text_pos;