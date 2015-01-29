
#ifndef _KeMemory32_h_
#define _KeMemory32_h_

#include "KeDef.h"
#include "KeGlobalVariables.h"
#include "KeKernelStruct32.h"
#include "KeCPUStruct32.h"
#define KeVirtualAddressToPhysicalAddress(SegmentBase,VirtualAddress) (ULONG32)((ULONG32)SegmentBase+(ULONG32)(VirtualAddress))


PVOID HYPKERNELAPI KeMemorySet(_IN_ PVOID Source,_IN_ BYTE Value,_IN_ ULONG32 Length);
PVOID HYPKERNELAPI _asm_KeMemoryCopy(_IN_ PVOID src,_IN_ PVOID dst,_IN_ ULONG32 size);
PVOID HYPKERNELAPI KeMemoryCopy(_IN_ PVOID src,_IN_ PVOID dst,_IN_ ULONG32 size);
VOID HYPKERNELAPI KeInitializeWritePDE4KDescriptor32(_OUT_ PKePDE4KDescriptor32 Desc,_IN_ ULONG32 P,_IN_ ULONG32 RW, _IN_ ULONG32 US,_IN_ ULONG32 PWT,_IN_ ULONG32 PCD,_IN_ ULONG32 A,_IN_ ULONG32 PS,_IN_ ULONG32 Address);
VOID HYPKERNELAPI KeWritePDE4K32(_IN_ PKePDE4KDescriptor32 Desc, _IN_ PVOID Dst);
VOID HYPKERNELAPI KeInitializeWritePTE4KDescriptor32(_OUT_ PKePTE4KDescriptor32 Desc,_IN_ ULONG32 P,_IN_ ULONG32 RW, _IN_ ULONG32 US,_IN_ ULONG32 PWT,_IN_ ULONG32 PCD,_IN_ ULONG32 A,_IN_ ULONG32 D,_IN_ ULONG32 PS,_IN_ ULONG32 G,_IN_ ULONG32 Address);
VOID HYPKERNELAPI KeWritePTE4K32(_IN_ PKePTE4KDescriptor32 Desc,_IN_ PVOID Dst );
VOID HYPKERNELAPI KeGetPDEInfo(_OUT_ PKePDE4KDescriptor32 pDesc, _IN_ PVOID PDEPtr);
VOID HYPKERNELAPI KeGetPTEInfo(_OUT_ PKePTE4KDescriptor32 pDesc, _IN_ PVOID PTEPtr);
ULONG32 HYPKERNELAPI KeGetPDEIndex4K32(_IN_ ULONG32 Virtual_Address);
ULONG32 HYPKERNELAPI KeGetPTEIndex4K32(_IN_ ULONG32 Virtual_Address);
ULONG32 HYPKERNELAPI MmMapVirtualAddress4K32(_IN_ PVOID PDEPtr, _IN_ ULONG32 PhysicalAddress, _IN_ ULONG32 VirtualAddress, _IN_ PKePTE4KDescriptor32 PTEDesc, _IN_ _OPTIONAL_ PKePDE4KDescriptor32 PDEDesc);
ULONG32 HYPKERNELAPI KeSetBit(_IN_ PVOID Source, _IN_ ULONG32 Position, _IN_ ULONG32 Value);
ULONG32 HYPKERNELAPI KeGetBit(_IN_ PVOID Source, _IN_ ULONG32 Position);


#define KeWritePDE4K32_UNDEFINED_BIT 3
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

#define KeWritePTE4K32_UNDEFINED_BIT 3
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

#endif