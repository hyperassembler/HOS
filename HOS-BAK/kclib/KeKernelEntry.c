#include "KeDef.h"
#include "KeGlobalVariables.h"
#include "KeGraph32.h"
#include "KeMemory32.h"
#include "KeIO32.h"
#include "KeProcess32.h"

VOID KeSystemInit(VOID);
VOID KeRawDelay(ULONG32 time);
VOID ProcessA(VOID);


char TempStack[PROCESS_STACK_MAX_SIZE];
VOID KeSystemInit(VOID)
{
	ULONG32 i;
	KeRawDelay(3500);
	_asm_KeClearGraph32();
	_asm_KeSetGraphPosition32(KeGetCurrentGraphRow(_asm_KeGetCurrentGraphPosition32()),28);
	KeRawDelay(500);
	_asm_KePrintStr32("=Hyper Operating System=\n");
	KeRawDelay(500);
	_asm_KePrintStr32("New GDT Loaded\n");
	KeRawDelay(500);
	_asm_KePrintStr32("Kernel Entry Entered.\n");
	KeRawDelay(500);
	_asm_KePrintStr32("Initializing Interrupt...\n");
	KeRawDelay(500);
	KeInitInterrupt();
	_asm_KePrintStr32("Interrupt Initialization Completed.\n");
	_asm_KePrintStr32("Testing Scroll...\n");
	KeRawDelay(1000);
	for(i=0;i<=30;i++)
	{
		_asm_KePrintStr32("This is text #");
		_asm_KePrintHex32(i);
		_asm_KePrintStr32(", ");
		_asm_KePrintHex32(30-i);
		_asm_KePrintStr32(" left.\n");
		KeRawDelay(50);
	}
	_asm_KePrintStr32("Scroll Test Completed.\n");
	KeRawDelay(500);
	_asm_KePrintStr32("Initializing Process Configurations...\n");
	KeInitProcess();
	KeRawDelay(500);
	_asm_KePrintStr32("Initializing Process Completed.\n");
	KeRawDelay(500);
	_asm_KePrintStr32("Try to run a process.\n");
	KeCreateProcess(0,&ProcessA,&TempStack[0],Process_Priviliege_Kernel);
shit:	goto shit;
}

#define PTEBase 0x00045000
#define PDEBase 0x00046000

VOID ProcessA(VOID)
{
	ULONG32 i = 0;
	while(i<1000)
	{
		_asm_KePrintHex32(i);
		KeRawDelay(250);
		i++;
	}
}

VOID KeRawDelay(ULONG32 time)
{
	ULONG32 i,j;
	for(i=0;i<=time;i++)
	{
		for(j=0;j<=10000;j++)
		{
			j=j;
		}
	}
}
