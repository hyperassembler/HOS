#include "type.h"
#include "kdef.h"
#include "print.h"
#include "mem.h"
#include "multiboot.h"
#include "../../../x86/src/c/print.h"

extern uint64_t text_pos;
extern void HLT_CPU(void);
extern void BOCHS_MAGIC_BREAKPOINT();
void HYPKERNEL64 hk_main(void)
{
    hk_clear_screen();
    hk_printf("Welcome to HYP OS. Kernel is now running in x64 mode.\n");
    HLT_CPU();
}
