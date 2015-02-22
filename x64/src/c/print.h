#ifndef _PRINT_H_
#define _PRINT_H_
#include "type.h"
#include "kdef.h"

#define get_column(pos) (pos % 80)
#define get_row(pos) (pos / 80)
#define get_pos(row,col) ((row) * 80 + (col))

void NATIVE64 clear_screen(void);
uint64_t NATIVE64 str_len(char const *str);
uint64_t NATIVE64 str_cmp(char const *str1, char const *str2);
void NATIVE64 kprintf(char const *format, ...);

#endif