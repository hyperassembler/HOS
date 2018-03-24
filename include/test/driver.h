#ifndef TEST_DRIVER_H
#define TEST_DRIVER_H

#include "type.h"

void SXAPI test_begin(char *name);

void SXAPI test_end(void);

void *SXAPI talloc(uint32 size);

void SXAPI run_case(char *name, bool result);

#endif
