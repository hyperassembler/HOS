#include "type.h"
#include "kernel/ke/print.h"
#include "kernel/ke/bug_check.h"

void KABI ke_bug_check(uint64_t reason)
{
    ke_printf("BugCheck: Reason - %ul\n", reason);
    while(true){};
}