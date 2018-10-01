#ifndef TEST_DRIVER_H
#define TEST_DRIVER_H

#include "common.h"

void
test_begin(char *name);

void
test_end(void);

void *
talloc(uint32 size);

void
run_case(char *name, bool result);

#endif
