#ifndef _KeKernelStruct32_h_
#define _KeKernelStruct32_h_
#include "KeDef.h"
#pragma  pack(push)
#pragma  pack(4)
typedef struct {
	ULONG32 Base; //+0
	ULONG32 Limit; //+4
	ULONG32 AVL; //+8
	ULONG32 DB; // +12
	ULONG32 P; //+16
	ULONG32 DPL; // +20
	ULONG32 G; //+24
	ULONG32 S; //+28
	ULONG32 Type; //+32
} KeGDTDescriptor32, *PKeGDTDescriptor32;

typedef struct {
	ULONG32 D;
	ULONG32 Offset;
	ULONG32 Selector;
	ULONG32 DPL;
	ULONG32 P;
	ULONG32 GateType;
} KeIDTDescriptor32, *PKeIDTDescriptor32;

typedef struct _WritePDE4K32_Descriptor
{
	ULONG32 P;
	ULONG32 RW;
	ULONG32 US;
	ULONG32 PWT;
	ULONG32 PCD;
	ULONG32 A;
	ULONG32 PS;
	ULONG32 Address;
} KePDE4KDescriptor32, *PKePDE4KDescriptor32;

typedef struct {
	ULONG32 P;
	ULONG32 RW;
	ULONG32 US;
	ULONG32 PWT;
	ULONG32 PCD;
	ULONG32 A;
	ULONG32 D;
	ULONG32 PS;
	ULONG32 G;
	ULONG32 Address;
} KePTE4KDescriptor32, *PKePTE4KDescriptor32;

#pragma  pack(pop)
#endif