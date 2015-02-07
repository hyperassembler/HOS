#ifndef _PRINT_H_
#define _PRINT_H_
#include "type.h"
#include "kdef.h"

#define get_column(pos) (pos % 80)
#define get_row(pos) (pos / 80)
#define get_pos(row,col) ((row) * 80 + (col))

void HYPKERNEL64 hk_clear_screen(void);
uint64_t HYPKERNEL64 hk_str_len(char const * str);
uint64_t HYPKERNEL64 hk_str_cmp(char const * str1,char const * str2);
void HYPKERNEL64 hk_printf(char const *format, ...);

#endif