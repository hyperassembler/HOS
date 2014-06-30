#include "KeProcess32.h"
#include "KeMemory32.h"
#include "KeKernelStruct32.h"
#include "KeIO32.h"
#include "KeGraph32.h"

VOID KeInitProcess(VOID)
{
	//TSS
	KeGDTDescriptor32 desc;
	KeMemorySet((PVOID)&KeTSS,0,sizeof(IATSS32));
	KeTSS.ss0 = GDT_SELECTOR_DATA;
	//DESC INIT

	KeInitializeKeGDTDescriptor32(&desc, (ULONG32)&KeTSS, sizeof(IATSS32)-1, KeWriteGDT_G_0,KeWriteGDT_P_1 , KeWriteGDT_S_0, KeWriteGDT_Type_System_32BitsTSSAvailable, KeWriteGDT_DPL_0, KeWriteGDT_DB_1, KeWriteGDT_AVL_0);

	KeWriteGDT(GDT_INDEX_TSS, &KeGDT[0], &desc);
	KeTSS.IOBase = sizeof(IATSS32)-1;
	_asm_KeLoadTSS(GDT_SELECTOR_TSS);
}

VOID KeCreateProcess(_IN_ ULONG32 ProcessID, _IN_ PVOID ProcessAddress, _IN_ PVOID ProcessStack, _IN_ PROCESSPRIVILEGE Privilege)
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