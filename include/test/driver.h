#ifndef _TEST_DRIVER_H_
#define _TEST_DRIVER_H_

#include "type.h"

void SXAPI test_begin(char *name);

void SXAPI test_end(void);

void *SXAPI talloc(uint32_t size);

void SXAPI run_case(char *name, bool result);

#endif
