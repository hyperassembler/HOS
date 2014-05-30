#include "KeDef.h"
#include "KeGlobalVariables.h"
#ifndef _KeMemory32_h_
#define _KeMemory32_h_
#define KeVirtualAddressToPhysicalAddress(SegmentBase,VirtualAddress) (ULONG32)((ULONG32)SegmentBase+(ULONG32)(VirtualAddress))

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
} WritePDE4K32_Descriptor, *PWritePDE4K32_Descriptor;

typedef struct _WritePTE4K32_Descriptor
{
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
} WritePTE4K32_Descriptor, *PWritePTE4K32_Descriptor;

PVOID _asm_KeMemoryCopy(PVOID src,PVOID dst,ULONG32 size);
PVOID KeMemoryCopy(PVOID src,PVOID dst,ULONG32 size);
VOID KeInitializeWritePDE4K32Descriptor(PWritePDE4K32_Descriptor Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 PS,ULONG32 Address);
VOID KeWritePDE4K32(PWritePDE4K32_Descriptor Desc, PVOID Dst);
VOID KeInitializeWritePTE4K32Descriptor(PWritePTE4K32_Descriptor Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 D,ULONG32 PS,ULONG32 G,ULONG32 Address);
VOID KeWritePTE4K32(PWritePTE4K32_Descriptor Desc,PVOID Dst );

ULONG32 SegmentToPhyscicalAddress(ULONG16 Segment)
{
	PGDT_DESCRIPTOR PDescriptor = &KeGDT[(Segment>>3)];
	return (PDescriptor->BaseHigh<<24 | PDescriptor->BaseMid<<16 | PDescriptor->BaseLow);
};
PVOID KeMemorySet(PVOID Source,BYTE Value,ULONG32 Length)
{
	PVOID src = Source;
	while(Length > 0)
	{
		*(PBYTE)Source = Value;
		Source++;
		Length--;
	};
	return src;
}
PVOID KeMemoryCopy(PVOID src,PVOID dst,ULONG32 size)
{
	ULONG32 i = 0;
	for(i=0;i<size;i++)
	{
		*((PCHAR)dst+i)=*((PCHAR)src+i);
	}
	return dst;
}

#define KeWritePDE4K32_P_0 0 // 0 for not present
#define KeWritePDE4K32_P_1 1 // 1 for Present
#define KeWritePDE4K32_RW_0 0 // 0 for ReadOnly
#define KeWritePDE4K32_RW_1 2 // 1 for ReadWrite
#define KeWritePDE4K32_US_0 0 // 0 for Superuser
#define KeWritePDE4K32_US_1 4 // 1 for All
#define KeWritePDE4K32_PWT_1 8 // Write back
#define KeWritePDE4K32_PWT_0 0 // Write Through
#define KeWritePDE4K32_PCD_0 0 // 0 for Cache Enabled
#define KeWritePDE4K32_PCD_1 16 // 1 for Cache Disabled
#define KeWritePDE4K32_A_0 0 //0 for not accessed
#define KeWritePDE4K32_A_1 32 //1 for accessed
#define KeWritePDE4K32_PS_0 0 //0 for 4K pages
#define KeWritePDE4K32_PS_1 128 //1 for 4M pages

VOID KeInitializeWritePDE4K32Descriptor(PWritePDE4K32_Descriptor Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 PS,ULONG32 Address)
{
    Desc->P = P;
    Desc->Address = Address;
    Desc->A = A;
    Desc->PS = PS;
    Desc->US = US;
    Desc->RW = RW;
    Desc->PWT = PWT;
    Desc->PCD = PCD;
    return;
}

VOID KeWritePDE4K32(PWritePDE4K32_Descriptor Desc, PVOID Dst)
{
    ULONG32 temp = 0;
    temp = Desc->Address;
    temp = (temp & 0xfffff000);
    temp = temp + Desc->P;
    temp = temp + Desc->RW;
    temp = temp + Desc->US;
    temp = temp + Desc->PWT;
    temp = temp + Desc->PCD;
    temp = temp + Desc->A;
    temp = temp + Desc->PS;
    *(PULONG32)Dst = temp;
    return;
}

#define KeWritePTE4K32_P_0 0 // 0 for not present
#define KeWritePTE4K32_P_1 1 // 1 for present
#define KeWritePTE4K32_RW_0 0 // 0 for readonly
#define KeWritePTE4K32_RW_1 2 // 1 for readwrite
#define KeWritePTE4K32_US_0 0 // 0 for superuser
#define KeWritePTE4K32_US_1 4 // 1 for all
#define KeWritePTE4K32_PWT_0 0 // 0 for write through
#define KeWritePTE4K32_PWT_1 8 // 1 for write back
#define KeWritePTE4K32_PCD_0 0 // 0 for cache enabled
#define KeWritePTE4K32_PCD_1 16 // 1 for cache disabled
#define KeWritePTE4K32_A_0 0 // not accessed
#define KeWritePTE4K32_A_1 32 // accessed
#define KeWritePTE4K32_D_0 0 // Dirty - set for has been written to
#define KeWritePTE4K32_D_1 64 // Dirty - set for has been written to
#define KeWritePTE4K32_PS_0 0 // should be 0
#define KeWritePTE4K32_PS_1 128 //
#define KeWritePTE4K32_G_0 0 // Global Not Set
#define KeWritePTE4K32_G_1 256 // Global Set


VOID KeInitializeWritePTE4K32Descriptor(PWritePTE4K32_Descriptor Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 D,ULONG32 PS,ULONG32 G,ULONG32 Address)
{
    Desc->P = P;
    Desc->Address = Address;
    Desc->A = A;
    Desc->US = US;
    Desc->RW = RW;
    Desc->PWT = PWT;
    Desc->PCD = PCD;
    Desc->PS = PS;
    Desc->G = G;
    Desc->D = D;
    return;
}

VOID KeWritePTE4K32(PWritePTE4K32_Descriptor Desc,PVOID Dst )
{
    ULONG32 temp = 0;
    temp = Desc->Address;
    temp = (temp & 0xfffff000);
    temp += Desc->P;
    temp += Desc->RW;
    temp += Desc->US;
    temp += Desc->PWT;
    temp += Desc->PCD;
    temp += Desc->A;
    temp += Desc->D;
    temp += Desc->PS;
    temp += Desc->G;
    *(PULONG32)Dst = temp;
}


#endif