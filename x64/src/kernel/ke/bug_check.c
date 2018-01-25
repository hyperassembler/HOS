#include "print.h"
#include "../../common/inc/abi.h"
#include "../../common/inc/type.h"
#include "bug_check.h"

void KABI ke_bug_check(uint64_t reason)
{
    ke_printf("BugCheck: Reason - %ul\n", reason);
    ke_halt_cpu();
}