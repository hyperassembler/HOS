#ifndef _KDEF_H_
#define _KDEF_H_

#define HYPKERNEL64 __attribute__((sysv_abi))

#define BOCHS_MAGIC_BREAKPOINT asm("xchg bx,bx");

#define SEG_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#define HLT_CPU asm("hlt");

#define NULL ((void*)0)

#endif
