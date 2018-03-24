#include "type.h"
#include "kernel/ke/print.h"
#include "kernel/ke/bug_check.h"

void SXAPI SXTRAP ke_trap(void)
{
	while (TRUE)
	{};
}

void SXAPI SXTRAP ke_panic(uint64 reason)
{
	ke_printf("BugCheck: Reason - %ul\n", reason);
	ke_trap();
}
