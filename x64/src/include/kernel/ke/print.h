#ifndef _KE_PRINT_H_
#define _KE_PRINT_H_

#include "abi.h"

void KABI ke_printf(const char* str, ...);
void KABI ke_vprintf(const char* str, va_list args);

#endif