#include "bifrost_statuscode.h"
#include "bifrost_test.h"
#include "bifrost_lock.h"
#include "bifrost_print.h"

static const char* _cur_test_name;
static int _total_num;

static int _failed_num;
static const char* _failed_cases[BIFROST_MAX_CASE_NUM];
static hw_result_t _failed_result[BIFROST_MAX_CASE_NUM];

static hw_spin_lock_t _test_lock;

void hw_start_test(const char* test_name)
{
    ke_spin_lock(&_test_lock);
    _failed_num = 0;
    _total_num = 0;
    _cur_test_name = test_name == NULL ? "Unnamed Test" : test_name;
    ke_spin_unlock(&_test_lock);
    hw_printf("%s started...\n", _cur_test_name);
}

void hw_run_case(const char* case_name, hw_result_t result)
{
    ke_spin_lock(&_test_lock);
    if(!HW_SUCCESS(result))
    {
        _failed_cases[_failed_num] = case_name == NULL ? "Unnamed Case" : case_name;
        _failed_result[_failed_num] = result;
        _failed_num++;
    }
    _total_num++;
    ke_spin_unlock(&_test_lock);
}

void hw_end_test()
{
    ke_spin_lock(&_test_lock);
    hw_printf("%s %s.\n", _cur_test_name, _failed_num > 0 ? "FAILED" : "PASSED");
    hw_printf("  Total number of cases: %d. %d failed.\n", _total_num, _failed_num);

    while(_failed_num > 0)
    {
        hw_printf("    %s failed with %d.\n", _failed_cases[_failed_num - 1], _failed_result[_failed_num - 1]);
        _failed_num--;
    }
    ke_spin_unlock(&_test_lock);
    return;
}
