#ifndef _KDEF_H_
#define _KDEF_H_

#define NATIVE32 __attribute__((cdecl))

#define NULL ((void*)0)

#define SEG_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#endif
