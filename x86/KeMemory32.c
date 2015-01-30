#include "KeMemory32.h"
#include "KeDef.h"
#include "KeKernelStruct32.h"
#include "KeGraph32.h"

PVOID HYPKERNELAPI KeMemorySet(_IN_ PVOID Source, _IN_ BYTE Value, _IN_ ULONG32 Length)
{
	PBYTE src = (PBYTE)Source;
	while (Length > 0)
	{
		_asm_KePrintHex32(Length);
		_asm_KePrintStr32("\n");
		*src = Value;
		src++;
		Length--;
	};
	return 0;
}

PVOID HYPKERNELAPI KeMemoryCopy(_IN_ PVOID src, _IN_ PVOID dst, _IN_ ULONG32 size)
{
	ULONG32 i = 0;
	for (i = 0; i < size; i++)
	{
		*((PCHAR)dst + i) = *((PCHAR)src + i);
	}
	return dst;
}


VOID HYPKERNELAPI KeInitializeWritePDE4KDescriptor32(_OUT_ PKePDE4KDescriptor32 Desc, _IN_ ULONG32 P, _IN_ ULONG32 RW, _IN_ ULONG32 US, _IN_ ULONG32 PWT, _IN_ ULONG32 PCD, _IN_ ULONG32 A, _IN_ ULONG32 PS, _IN_ ULONG32 Address)
{
	if (Desc == NULL)
		return;
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

VOID HYPKERNELAPI KeWritePDE4K32(_IN_ PKePDE4KDescriptor32 Desc, _IN_ PVOID Dst)
{
	if (Desc == NULL || Dst == NULL)
		return;
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


VOID HYPKERNELAPI KeInitializeWritePTE4KDescriptor32(_OUT_ PKePTE4KDescriptor32 Desc, _IN_ ULONG32 P, _IN_ ULONG32 RW, _IN_ ULONG32 US, _IN_ ULONG32 PWT, _IN_ ULONG32 PCD, _IN_ ULONG32 A, _IN_ ULONG32 D, _IN_ ULONG32 PS, _IN_ ULONG32 G, _IN_ ULONG32 Address)
{
	if (Desc == NULL)
		return;
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

VOID HYPKERNELAPI KeGetPDEInfo( _OUT_ PKePDE4KDescriptor32 pDesc, _IN_ PVOID PDEPtr)
{
	if (pDesc == NULL || PDEPtr == NULL)
		return;
	ULONG32 temp = *(PULONG32)PDEPtr;
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

VOID HYPKERNELAPI KeGetPTEInfo( _OUT_ PKePTE4KDescriptor32 pDesc, _IN_ PVOID PTEPtr)
{
	if (pDesc == NULL || PTEPtr == NULL)
		return;
	ULONG32 temp = *(PULONG32)PTEPtr;
	pDesc->Address = (temp >> 12) << 12;
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

VOID HYPKERNELAPI KeWritePTE4K32( _IN_ PKePTE4KDescriptor32 Desc, _IN_ PVOID Dst)
{
	if (Desc == NULL || Dst == NULL)
		return;
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

ULONG32 HYPKERNELAPI KeGetPDEIndex4K32( _IN_ ULONG32 VirtualAddress)
{
	return VirtualAddress >> 22;
}

ULONG32 HYPKERNELAPI KeGetPTEIndex4K32( _IN_ ULONG32 VirtualAddress)
{
	return (VirtualAddress << 10) >> 22;
}



ULONG32 HYPKERNELAPI MmMapVirtualAddress4K32(_IN_ PVOID PDEBase, _IN_ ULONG32 PhysicalAddress, _IN_ ULONG32 VirtualAddress, _IN_ PKePTE4KDescriptor32 PTEDesc, _IN_ _OPTIONAL_  PKePDE4KDescriptor32 PDEDesc)
{
	//Physical Address and Virtual Address will be trimmed to be 4K-aligned
	//Please assign proper address for PDEDesc
	KePDE4KDescriptor32 PDE;
	ULONG32 PDEIndex = KeGetPDEIndex4K32(VirtualAddress);
	ULONG32 PTEIndex = KeGetPTEIndex4K32(VirtualAddress);
	KeGetPDEInfo(&PDE, (PVOID)(PDEIndex*4 + (ULONG32)PDEBase));
	if (PDE.P == KeWritePDE4K32_P_0)
	{
		KeWritePDE4K32(PDEDesc, (PVOID)(PDEIndex * 4 + (ULONG32)PDEBase));
	}
	ULONG32 PTEBase = (*(PULONG32)((ULONG32)PDEBase + PDEIndex * 4) >> 12) << 12;
	PVOID Target = (PVOID)((*(PULONG32)(PTEBase + PTEIndex * 4) >> 12) << 12);
	KePTE4KDescriptor32 PTE;
	KeGetPTEInfo(&PTE, Target);
	PTEDesc->Address = VirtualAddress;
	KeWritePTE4K32(PTEDesc, Target);
	return 0;
}


ULONG32 HYPKERNELAPI KeGetBit(_IN_ PVOID Source, _IN_ ULONG32 Position)
{
	if (!Source)
		return 0;
	ULONG32 Quotient = Position / 8;
	ULONG32 Remainder = Position % 8;
	PULONG8 CurrentPos = (PULONG8)Source;
	ULONG8 Value;
	while (Quotient > 0)
	{
		CurrentPos++;
		Quotient--;
	}
	Value = *CurrentPos;
	Value = Value << (7 - Remainder);
	Value = Value >> 7;
	return Value;
}

ULONG32 HYPKERNELAPI KeSetBit(_IN_ PVOID Source, _IN_ ULONG32 Position, _IN_ ULONG32 Value)
{
	if (!Source || Position < 0 || (Value != 0 && Value != 1))
		return 1;
	ULONG32 Quotient = Position / 8;
	ULONG32 Remainder = Position % 8;
	PULONG8 CurrentPos = (PULONG8)Source;
	while (Quotient > 0)
	{
		CurrentPos++;
		Quotient--;
	}
	if (Value == 1)
	{
		ULONG8 temp = 1;
		temp = temp << Remainder;
		*CurrentPos = *CurrentPos | temp;
	}
	else
	{
		ULONG8 temp = 0xfe;
		while (Remainder > 0)
		{
			temp = temp << 1;
			temp++;
			Remainder--;
		}
		*CurrentPos = *CurrentPos & temp;
	}
	return 0;
}