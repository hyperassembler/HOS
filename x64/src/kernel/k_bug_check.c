#include "g_abi.h"
#include "g_type.h"
#include "k_bug_check.h"

void KAPI k_bug_check(uint64_t reason)
{
    k_halt_cpu();
}