#ifndef _S_PRINT_H_
#define _S_PRINT_H_

#include "abi.h"
#include "type.h"

void KABI hal_printf(const char* str, ...);
void KABI hal_clear_screen();
void KABI hal_print_init();

#endif