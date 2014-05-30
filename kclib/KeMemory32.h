
#ifndef _KeMemory32_h_
#define _KeMemory32_h_

#include "KeDef.h"
#include "KeGlobalVariables.h"
#include "KeKernelStruct32.h"
#include "KeCPUStruct32.h"
#define KeVirtualAddressToPhysicalAddress(SegmentBase,VirtualAddress) (ULONG32)((ULONG32)SegmentBase+(ULONG32)(VirtualAddress))

PVOID  KeNativeAPI KeMemorySet(PVOID Source,BYTE Value,ULONG32 Length);
PVOID  KeNativeAPI _asm_KeMemoryCopy(PVOID src,PVOID dst,ULONG32 size);
PVOID  KeNativeAPI KeMemoryCopy(PVOID src,PVOID dst,ULONG32 size);
VOID  KeNativeAPI KeInitializeWritePDE4KDescriptor32(PKePDE4KDescriptor32 Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 PS,ULONG32 Address);
VOID  KeNativeAPI KeWritePDE4K32(PKePDE4KDescriptor32 Desc, PVOID Dst);
VOID  KeNativeAPI KeInitializeWritePTE4KDescriptor32(PKePTE4KDescriptor32 Desc,ULONG32 P,ULONG32 RW, ULONG32 US,ULONG32 PWT,ULONG32 PCD,ULONG32 A,ULONG32 D,ULONG32 PS,ULONG32 G,ULONG32 Address);
VOID  KeNativeAPI KeWritePTE4K32(PKePTE4KDescriptor32 Desc,PVOID Dst );
ULONG32 KeNativeAPI KeGetPDEIndex4K32(ULONG32 Virtual_Address);
ULONG32 KeNativeAPI KeGetPTEIndex4K32(ULONG32 Virtual_Address);
ULONG32 KeNativeAPI KeSetMappingAddress(PVOID PDE_Dir_Address,ULONG32 Physical_Address,ULONG32 Virtual_Address);


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