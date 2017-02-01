#ifndef _K_TEST_DRIVER_H_
#define _K_TEST_DRIVER_H_

#include "g_type.h"
#include "g_abi.h"

void KABI test_begin(char *name);

void KABI test_end();

void *KABI talloc(uint32_t size);

void KABI run_case(char *name, bool result);

#endif
