#include <kern/panic.h>
#include <kern/print.h>

void ke_panic(uint32 reason)
{
    ke_printf("BugCheck: Reason - %ul\n", reason);
    hal_halt();
}
