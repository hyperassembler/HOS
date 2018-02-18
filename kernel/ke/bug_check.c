#include "type.h"
#include "kernel/ke/print.h"
#include "kernel/ke/bug_check.h"

void KABI ke_trap(void)
{
	while (true)
	{};
}

void KABI ke_panic(uint64_t reason)
{
	ke_printf("BugCheck: Reason - %ul\n", reason);
	ke_trap();
}