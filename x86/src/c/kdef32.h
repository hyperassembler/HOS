#ifndef _HKDEF32_H_
#define _HKDEF32_H_

#define HYPKERNEL32 __attribute__((cdecl))

#define BOCHS_MAGIC_BREAKPOINT asm("xchg bx,bx");

#define NULL ((void*)0)

#define SEGMENT_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#endif
