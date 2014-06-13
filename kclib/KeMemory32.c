#include "KeMemory32.h"
#include "KeDef.h"
#include "KeGraph32.h"

ULONG32 HYPKERNELAPI SegmentToPhyscicalAddress(ULONG16 Segment)
{
	PIAGDT32 PDescriptor = &KeGDT[(Segment>>3)];
	return (PDescriptor->BaseHigh<<24 | PDescriptor->BaseMid<<16 | PDescriptor->BaseLow);
};

PVOID HYPKERNELAPI KeMemorySet(PVOID Source, BYTE Value, ULONG32 Length)
{
	PBYTE src = (PBYTE)Source;
	while(Length > 0)
	{
		_asm_KePrintHex32(Length);
		_asm_KePrintStr32("\n");
		*src = Value;
		src++;
		Length--;
	};
	return 0;
}

PVOID HYPKERNELAPI KeMemoryCopy(PVOID src, PVOID dst, ULONG32 size)
{
	ULONG32 i = 0;
	for(i=0;i<size;i++)
	{
		*((PCHAR)dst+i)=*((PCHAR)src+i);
	}
	return dst;
}


VOID HYPKERNELAPI KeInitializeWritePDE4KDescriptor32(PKePDE4KDescriptor32 Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 PS,ULONG32 Address)
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

VOID HYPKERNELAPI KeWritePDE4K32(PKePDE4KDescriptor32 Desc, PVOID Dst)
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


VOID HYPKERNELAPI KeInitializeWritePTE4K32Descriptor(PKePTE4KDescriptor32 Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 D,ULONG32 PS,ULONG32 G,ULONG32 Address)
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

VOID HYPKERNELAPI KeGetPDEInfo(PKePDE4KDescriptor32 pDesc,ULONG32 Virtual_Address)
{
	ULONG32 temp = *(PULONG32)Virtual_Address;
	pDesc->Address = (temp >> 12) << 12;
	pDesc->A = temp & KeWritePDE4K32_A_1;
	pDesc->P = temp & KeWritePDE4K32_P_1;
	pDesc->PCD = temp & KeWritePDE4K32_PCD_1;
	pDesc->PS = temp & KeWritePDE4K32_PS_1;
	pDesc->US = temp & KeWritePDE4K32_US_1;
	pDesc->RW = temp & KeWritePDE4K32_RW_1;
	pDesc->PWT = temp & KeWritePDE4K32_PWT_1;
	return;
}

VOID HYPKERNELAPI KeGetPTEInfo(PKePTE4KDescriptor32 pDesc,ULONG32 Virtual_Address)
{
	ULONG32 temp = *(PULONG32)Virtual_Address;
	pDesc->Address = (temp>>12)<<12;
	pDesc->P = temp & KeWritePTE4K32_P_1;
	pDesc->D = temp & KeWritePTE4K32_D_1;
	pDesc->US = temp & KeWritePTE4K32_US_1;
	pDesc->PCD = temp & KeWritePTE4K32_PCD_1;
	pDesc->PWT = temp & KeWritePTE4K32_PWT_1;
	pDesc->PS = temp & KeWritePTE4K32_PS_1;
	pDesc->RW = temp & KeWritePTE4K32_RW_1;
	pDesc->G = temp & KeWritePTE4K32_G_1;
	return;
}

VOID HYPKERNELAPI KeWritePTE4K32(PKePTE4KDescriptor32 Desc,PVOID Dst )
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

ULONG32 HYPKERNELAPI KeGetPDEIndex4K32(ULONG32 Virtual_Address)
{
	return Virtual_Address >> 22;
}

ULONG32 HYPKERNELAPI KeGetPTEIndex4K32(ULONG32 Virtual_Address)
{
	return (Virtual_Address << 10) >> 22;
}



ULONG32 HYPKERNELAPI MmMapVirtualAddress(PIAPDE32 PDE_Ptr, ULONG32 PhysicalAddress, ULONG32 VirtualAddress, PKePTE4KDescriptor32 PTEDesc)
{
	ULONG32 PDEIndex = KeGetPDEIndex4K32(VirtualAddress);
	ULONG32 PTEIndex = KeGetPDEIndex4K32(VirtualAddress);
	ULONG32 PTEBase = (*(PULONG32)((ULONG32)PDE_Ptr + PDEIndex * 4) >> 12) << 12; // PTE Address
	PVOID Target = (PVOID)((*(PULONG32)(PTEBase + PTEIndex * 4) >> 12) << 12);
	//this is test
	return 0;
}