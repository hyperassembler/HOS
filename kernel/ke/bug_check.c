#include "kp.h"

void ke_panic(uint64 reason)
{
    ke_printf("BugCheck: Reason - %ul\n", reason);
    hal_halt();
}
