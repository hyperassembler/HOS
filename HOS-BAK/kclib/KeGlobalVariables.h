#include "KeDef.h"
#ifndef _KeGlobalVariables_h_
#define _KeGlobalVariables_h_
ULONG8 KeGDTPtr[6];
GDT_DESCRIPTOR KeGDT[GDT_DESCRIPTOR_NUMBER];
ULONG8 KeIDTPtr[6];
TSS KeTSS;
GATE KeIDT[IDT_GATE_NUMBER];
PhProcess PStartProcess;
hPrcocess KeUserProcessTable[MAXIMUM_PROCESS_NUMBER];
hPrcocess KeKernelProcessTable[MAXIMUM_PROCESS_NUMBER];
#endif
//Global Variables
