#ifndef _K_TEST_DRIVER_H_
#define _K_TEST_DRIVER_H_

#include "../../common/inc/type.h"
#include "../../common/inc/abi.h"

void KABI test_begin(char *name);

void KABI test_end();

void *KABI talloc(uint32_t size);

void KABI run_case(char *name, bool result);

#endif
