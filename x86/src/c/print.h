#ifndef _PRINT_H_
#define _PRINT_H_
#include "type.h"
#include "kdef.h"

#define get_column(pos) (pos % 80)
#define get_row(pos) (pos / 80)
#define get_pos(row,col) ((row) * 80 + (col))

void HYPKERNEL32 hk_print_hex(uint32_t number);
void HYPKERNEL32 hk_print_int(int32_t number);
void HYPKERNEL32 hk_print_str(char const *str);
uint32_t HYPKERNEL32 hk_str_len(char const * str);
uint32_t HYPKERNEL32 hk_str_cmp(char const * str1,char const * str2);

#endif