#ifndef _BIFROST_TEST_H
#define _BIFROST_TEST_H

#include "bifrost_statuscode.h"

#define BIFROST_MAX_CASE_NUM 64

void hw_start_test(const char* test_name);

void hw_run_case(const char* case_name, hw_result_t result);

void hw_end_test();

// actual tests
void thread_test_main();

#endif
