#include "type.h"
#include "kdef.h"
#include "print.h"
#include "mem.h"

extern uint64_t text_pos;
void HYPKERNEL64 hk_main(void)
{
    hk_clear_screen();
    hk_print_str("Welcome to HYP OS. Kernel is now running in x64 mode.\n");
    x64:
    goto x64;
}
