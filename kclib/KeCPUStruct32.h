#ifndef _KeCPUStruct32_h_
#define _KeCPUStruct32_h_
#include "KeDef.h"
#pragma pack(push)
#pragma pack(1)

typedef struct {
	ULONG16 LimitLow;
	ULONG16 BaseLow;
	ULONG8 BaseMid;
	ULONG8 Attr1;
	ULONG8 LimitHigh_Attr2;
	ULONG8 BaseHigh;
} IAGDT32,*PIAGDT32;

typedef IAGDT32 IALDT32,*PIALDT32;

typedef ULONG32 IAPDE32,*PIAPDE32;

typedef ULONG32 IAPTE32,*PIAPTE32;

typedef struct {
	ULONG16 OffsetLow;
	ULONG16 Selector;
	ULONG8 Count;
	ULONG8 Attr;
	ULONG16 OffsetHigh;
} IAGATE32,*PIAGATE32;

typedef struct {
	ULONG32 BackLine;
	ULONG32	esp0;
	ULONG32	ss0;
	ULONG32	esp1;
	ULONG32	ss1;
	ULONG32	esp2;
	ULONG32	ss2;
	ULONG32	cr3;
	ULONG32	eip;
	ULONG32	flags;
	ULONG32	eax;
	ULONG32	ecx;
	ULONG32	edx;
	ULONG32	ebx;
	ULONG32	esp;
	ULONG32	ebp;
	ULONG32	esi;
	ULONG32	edi;
	ULONG32	es;
	ULONG32	cs;
	ULONG32	ss;
	ULONG32	ds;
	ULONG32	fs;
	ULONG32	gs;
	ULONG32	LDT;
	ULONG16	Trap;
	ULONG16	IOBase;
} IATSS32,*PIATSS32;

#pragma pack(pop)
#endif