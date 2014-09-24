#ifndef _HKDEF32_H_
#define _HKDEF32_H_

#define HKA32 _cdecl

#define NULL ((void*)0)

#define GDT_SELECTOR(Index,RPL) (((Index) << 3) + (RPL))

#define HKernelAddress (0x01000000)
#endif
