#ifndef _S_PRINT_H_
#define _S_PRINT_H_

#include "g_abi.h"
#include "g_type.h"

//TODO: Get rid of this
#include "hal_print.h"

#define ke_printf(x, ...) hal_printf(x, __VA_ARGS__)

#endif