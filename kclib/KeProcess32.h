#ifndef _KeProcess32_h_
#define _KeProcess32_h_

#include "KeDef.h"
#include "KeKernelStruct32.h"

#pragma  pack(push)
#pragma  pack(4)

typedef struct {
	ULONG32 gs;
	ULONG32 fs;
	ULONG32 es;
	ULONG32 ds;
	ULONG32 edi;
	ULONG32 esi;
	ULONG32 ebp;
	ULONG32 Kernel_esp;
	ULONG32 ebx;
	ULONG32 edx;
	ULONG32 ecx;
	ULONG32 eax;
	ULONG32 ReturnAddress;
	ULONG32 eip;
	ULONG32 cs;
	ULONG32 eFlags;
	ULONG32 esp;
	ULONG32 ss;
} hProcess_RestartStack,*PhProcess_RestartStack;

typedef ULONG32 PROCESSPRIVILEGE;
#define Process_Priviliege_User 0x01
#define Process_Priviliege_Kernel 0x00

typedef struct {
	hProcess_RestartStack RestartStack;
	//ULONG16 LDTSelector;
	//LDT_DESCRIPTOR ProcessLDT[LDT_DESCRIPTOR_NUMBER];
	ULONG32 ProcessID;
	PCHAR ProcessName;
	PROCESSPRIVILEGE Privilege;
} hPrcocess,*PhProcess;

#pragma  pack(pop)



VOID  HYPKERNELAPI KeCreateProcess(ULONG32 ProcessID,PVOID ProcessAddress,PVOID ProcessStack,PROCESSPRIVILEGE Privilege);
VOID  HYPKERNELAPI KeInitProcess(VOID);
VOID  HYPKERNELAPI _asm_KeLoadTSS(ULONG32 Selector_TSS);
VOID  HYPKERNELAPI _asm_Restart();
#endif