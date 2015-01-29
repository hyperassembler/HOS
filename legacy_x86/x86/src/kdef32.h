#ifndef _HKDEF32_H_
#define _HKDEF32_H_

#define HYPKERNEL32 __cdecl

#define NULL ((void*)0)

#define GDT_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#define HKERNEL_ADDR (0x01000000)
#endif
