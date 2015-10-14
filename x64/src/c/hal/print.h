#ifndef _PRINT_H_
#define _PRINT_H_
#include "../common/sys/kdef.h"
#include "../common/sys/type.h"

#define get_column(pos) (pos % 80)
#define get_row(pos) (pos / 80)
#define get_pos(row,col) ((row) * 80 + (col))

void _KERNEL_ABI hal_clear_screen(void);
void _KERNEL_ABI hal_printf(char const *format, ...);

#endif