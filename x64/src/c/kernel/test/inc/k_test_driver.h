#ifndef _K_TEST_DRIVER_H_
#define _K_TEST_DRIVER_H_

#include "k_type.h"
#include "k_def.h"

void SAPI test_begin(char *name);

void SAPI test_end();

void *SAPI talloc(uint32_t size);

void SAPI run_case(char *name, bool result);

#endif
