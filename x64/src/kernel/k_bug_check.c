#include "k_print.h"
#include "g_abi.h"
#include "g_type.h"
#include "k_bug_check.h"

void KAPI ke_bug_check(uint64_t reason)
{
    ke_printf("BugCheck: Reason - %ul\n", reason);
    ke_halt_cpu();
}