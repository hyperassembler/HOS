#include "kernel/ke/print.h"

void SXAPI ke_printf(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	ke_vprintf(str, args);
	va_end(args);
}

void SXAPI ke_vprintf(const char *str, va_list args)
{
	hal_vprintf(str, args);
}
