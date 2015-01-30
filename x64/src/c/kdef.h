#ifndef _KDEF_H_
#define _KDEF_H_

#define HYPKERNEL64 __attribute__((sysv_abi))

#define BOCHS_MAGIC_BREAKPOINT asm("xchg bx,bx");

#define NULL ((void*)0)

#endif
