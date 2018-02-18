#include "kernel/ke/print.h"

void KABI ke_printf(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	ke_vprintf(str, args);
	va_end(args);
	return;
}

void KABI ke_vprintf(const char *str, va_list args)
{
	hal_vprintf(str, args);
	return;
}