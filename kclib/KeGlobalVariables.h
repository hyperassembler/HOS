#ifndef _KeGlobalVariables_h_
#define _KeGlobalVariables_h_
#include "KeDef.h"
#include "KeCPUStruct32.h"
#include "KeProcess32.h"

ULONG8 KeGDTPtr[6];
IAGDT32 KeGDT[GDT_DESCRIPTOR_NUMBER];
ULONG8 KeIDTPtr[6];
IATSS32 KeTSS;
IAGATE32 KeIDT[IDT_GATE_NUMBER];
PhProcess PStartProcess;
hPrcocess KeUserProcessTable[MAXIMUM_PROCESS_NUMBER];
hPrcocess KeKernelProcessTable[MAXIMUM_PROCESS_NUMBER];
#endif
//Global Variables
