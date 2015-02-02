#ifndef _KDEF_H_
#define _KDEF_H_

#define HYPKERNEL32 __attribute__((cdecl))

#define BOCHS_MAGIC_BREAKPOINT asm("xchg bx,bx");
#define HLT_CPU asm("hlt");

#define NULL ((void*)0)

#define SEGMENT_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#endif
