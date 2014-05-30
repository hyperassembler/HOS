#include "KeDef.h"
#include "KeGlobalVariables.h"
#include "KeMemory32.h"
#include "KeGraph32.h"
#include "KeIO32.h"
#ifndef _KeProcess32_h_
#define _KeProcess32_h_
VOID KeCreateProcess(ULONG32 ProcessID,PVOID ProcessAddress,PVOID ProcessStack,PROCESSPRIVILEGE Privilege);
VOID KeInitProcess(VOID);
VOID _asm_KeLoadTSS(ULONG32 Selector_TSS);
VOID _asm_Restart();

VOID KeInitProcess(VOID)
{
	//TSS
    Write_GDT_DESCRIPTOR desc;
	KeMemorySet(&KeTSS,0,sizeof(TSS));
	KeTSS.ss0 = GDT_SELECTOR_DATA;
    //DESC INIT
    KeInitializeWriteGDTDescriptor(&desc, (ULONG32)&KeTSS, sizeof(TSS)-1, KeWriteGDT_G_0,KeWriteGDT_P_1 , KeWriteGDT_S_0, KeWriteGDT_Type_System_32BitsTSSAvailable, KeWriteGDT_DPL_0, KeWriteGDT_DB_1, KeWriteGDT_AVL_0);
    
	KeWriteGDT(GDT_INDEX_TSS, &KeGDT[0], &desc);
    
	KeTSS.IOBase = sizeof(TSS)-1;
    
	_asm_KeLoadTSS(GDT_SELECTOR_TSS);
}

VOID KeCreateProcess(ULONG32 ProcessID,PVOID ProcessAddress,PVOID ProcessStack,PROCESSPRIVILEGE Privilege)
{
	PhProcess process;
	if(Privilege == Process_Priviliege_Kernel)
	{
		process = &KeKernelProcessTable[ProcessID];
		process->ProcessID = ProcessID;
		process->Privilege = Privilege;
		process->RestartStack.cs = GDT_SELECTOR_CODE; //(LDT_SELECTOR_CODE & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.ds = GDT_SELECTOR_DATA; //(LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.es = GDT_SELECTOR_DATA; //(LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.fs = GDT_SELECTOR_DATA; //(LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.ss = GDT_SELECTOR_DATA; // (LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.gs = GDT_SELECTOR_GRAPH; // (GDT_SELECTOR_GRAPH & SA_RPL_MASK) | RPL_TASK;
	}
	else
	{
		process = &KeUserProcessTable[ProcessID];
		process->ProcessID = ProcessID;
		process->Privilege = Privilege;
		process->RestartStack.cs =GDT_SELECTOR_USER_CODE; //(LDT_SELECTOR_CODE & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.ds =GDT_SELECTOR_USER_DATA; //(LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.es =GDT_SELECTOR_USER_DATA; //(LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.fs =GDT_SELECTOR_USER_DATA; //(LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.ss =GDT_SELECTOR_USER_DATA; // (LDT_SELECTOR_DATA & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		process->RestartStack.gs =GDT_SELECTOR_GRAPH; // (GDT_SELECTOR_GRAPH & SA_RPL_MASK) | RPL_TASK;
	}
	process->RestartStack.eip = (ULONG32)ProcessAddress;
	process->RestartStack.esp = (ULONG32)ProcessStack + PROCESS_STACK_MAX_SIZE;
	process->RestartStack.eFlags = 0x1202;

//	process->LDTSelector = GDT_SELECTOR_LDT;
//	KeMemoryCopy(&KeGDT[GDT_INDEX_CODE],&process->ProcessLDT[LDT_INDEX_CODE],sizeof(GDT_DESCRIPTOR));
//	process->ProcessLDT[LDT_INDEX_CODE].Attr1 = DA_C | PRIVILEGE_TASK << 5;
//	KeMemoryCopy(&KeGDT[GDT_INDEX_DATA],&process->ProcessLDT[LDT_INDEX_DATA],sizeof(GDT_DESCRIPTOR));
//	process->ProcessLDT[LDT_INDEX_DATA].Attr1 = DA_DRW | PRIVILEGE_TASK << 5;
	PStartProcess = process;
	_asm_Restart();
}

#endif