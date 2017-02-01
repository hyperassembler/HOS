#include "print.h"
#include "g_abi.h"
#include "g_type.h"
#include "bug_check.h"

void KABI ke_bug_check(uint64_t reason)
{
    ke_printf("BugCheck: Reason - %ul\n", reason);
    ke_halt_cpu();
}