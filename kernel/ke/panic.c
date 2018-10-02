#include "ke/panic.h"
#include "ke/print.h"
#include "hal.h"

void ke_panic(uint32 reason)
{
    ke_printf("BugCheck: Reason - %ul\n", reason);
    hal_halt();
}
