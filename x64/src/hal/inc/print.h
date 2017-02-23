/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _HAL_PRINT_H_
#define _HAL_PRINT_H_
#include "g_abi.h"
#include "g_type.h"

#define get_column(pos) (pos % 80)
#define get_row(pos) (pos / 80)
#define get_pos(row,col) ((row) * 80 + (col))

void KABI hal_clear_screen();
void KABI hal_print_init();
void KABI hal_printf(char const *format, ...);

#endif