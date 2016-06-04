/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _HAL_PRINT_H_
#define _HAL_PRINT_H_
#include "k_def.h"
#include "k_type.h"

#define get_column(pos) (pos % 80)
#define get_row(pos) (pos / 80)
#define get_pos(row,col) ((row) * 80 + (col))

void KAPI hal_clear_screen(void);
void KAPI hal_printf(char const *format, ...);

#endif