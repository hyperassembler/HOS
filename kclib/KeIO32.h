#ifndef _KeIO32_h_
#define _KeIO32_h_

#include "KeDef.h"
#include "KeGlobalVariables.h"
#include "KeCPUStruct32.h"
#include "KeKernelStruct32.h"

#define	PRIVILEGE_KERNEL 0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

typedef VOID (*PExceptionHandler)(VOID);

VOID HYPKERNELAPI _asm_KeWritePort(_IN_ ULONG16 Port, _IN_ ULONG8 Value);
ULONG8 HYPKERNELAPI _asm_KeReadPort(_IN_ ULONG16 Port);
VOID HYPKERNELAPI _asm_KeLoadIDT(VOID);
VOID HYPKERNELAPI KeInit8259A(VOID);
VOID HYPKERNELAPI KeInitInterrupt(VOID);
VOID HYPKERNELAPI KeSwitchGDT(VOID);
VOID HYPKERNELAPI KeInitIDT(VOID);
VOID HYPKERNELAPI KeWriteGDT(_IN_ ULONG32 Index, _IN_ PIAGDT32 Dest, _IN_ PKeGDTDescriptor32 Src);
VOID HYPKERNELAPI KeWriteIDT(_IN_ ULONG32 Index, _IN_ PIAGATE32 Dest, _IN_ PKeIDTDescriptor32 Src);
VOID HYPKERNELAPI KeExceptionHandler(_IN_ ULONG32 VectorNumber, _IN_ ULONG32 ErrorCode, _IN_ ULONG32 eip, _IN_ ULONG32 cs, _IN_ ULONG32 eFlags);
VOID HYPKERNELAPI _asm_KeDivideError(VOID);
VOID HYPKERNELAPI _asm_KeSingleStepException(VOID);
VOID HYPKERNELAPI _asm_KeNMI(VOID);
VOID HYPKERNELAPI _asm_KeBreakpointException(VOID);
VOID HYPKERNELAPI _asm_KeOverflow(VOID);
VOID HYPKERNELAPI _asm_KeBoundsCheck(VOID);
VOID HYPKERNELAPI _asm_KeInvalidOpcode(VOID);
VOID HYPKERNELAPI _asm_KeCoprNotAvailable(VOID);
VOID HYPKERNELAPI _asm_KeDoubleFault(VOID);
VOID HYPKERNELAPI _asm_KeCoprSegmentOverrun(VOID);
VOID HYPKERNELAPI _asm_KeInvalidTSS(VOID);
VOID HYPKERNELAPI _asm_KeSegmentNotPresent(VOID);
VOID HYPKERNELAPI _asm_KeStackException(VOID);
VOID HYPKERNELAPI _asm_KeGeneralProtection(VOID);
VOID HYPKERNELAPI _asm_KePageFault(VOID);
VOID HYPKERNELAPI _asm_KeCoprError(VOID);
VOID HYPKERNELAPI KeIrqIntHandler(_IN_ ULONG32 IrqNumber);
VOID HYPKERNELAPI _asm_KeIrqInt00(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt01(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt02(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt03(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt04(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt05(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt06(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt07(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt08(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt09(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt10(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt11(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt12(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt13(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt14(VOID);
VOID HYPKERNELAPI _asm_KeIrqInt15(VOID);
VOID HYPKERNELAPI KeInitializeKeGDTDescriptor32(_OUT_ PKeGDTDescriptor32 Desc, _IN_ ULONG32 Base, _IN_ ULONG32 Limit, _IN_ ULONG32 G, _IN_ ULONG32 P, _IN_ ULONG32 S, _IN_ ULONG32 Type, _IN_ ULONG32 DPL, _IN_ ULONG32 DB, _IN_ ULONG32 AVL);
VOID HYPKERNELAPI KeInitializeKeIDTDescriptor32(_OUT_ PKeIDTDescriptor32 Desc, _IN_ ULONG32 GateType, _IN_ ULONG32 DPL, _IN_ ULONG32 D, _IN_ ULONG32 P, _IN_ ULONG32 Selector, _IN_ ULONG32 Offset);

//KeWriteGDT
#define KeWriteGDT_AVL_1 0x00100000
#define KeWriteGDT_AVL_0 0x0
#define KeWriteGDT_DB_0 0x0
#define KeWriteGDT_DB_1 0x00400000
#define KeWriteGDT_P_0 0x0
#define KeWriteGDT_P_1 0x00008000
#define KeWriteGDT_G_0 0x0
#define KeWriteGDT_G_1 0x00800000
#define KeWriteGDT_L_0 0x0
#define KeWriteGDT_L_1 0x00200000
#define KeWriteGDT_DPL_0 0x0
#define KeWriteGDT_DPL_1 0x00002000
#define KeWriteGDT_DPL_2 0x00004000
#define KeWriteGDT_DPL_3 0x00006000
#define KeWriteGDT_S_0 0x0
#define KeWriteGDT_S_1 0x00001000
#define KeWriteGDT_Type_Data_ReadOnly 0x0
#define KeWriteGDT_Type_Data_ReadOnlyAccessed 0x00000100
#define KeWriteGDT_Type_Data_ReadWrite 0x00000200
#define KeWriteGDT_Type_Data_ReadWriteAccessed 0x00000300
#define KeWriteGDT_Type_Data_ReadOnlyExpandDown 0x00000400
#define KeWriteGDT_Type_Data_ReadOnlyExpandDownAccessed 0x00000500
#define KeWriteGDT_Type_Data_ReadWriteExpandDown 0x00000600
#define KeWriteGDT_Type_Data_ReadWriteExpandDownAccessed 0x00000700
#define KeWriteGDT_Type_Code_ExecuteOnly 0x00000800
#define KeWriteGDT_Type_Code_ExecuteOnlyAccessed 0x00000900
#define KeWriteGDT_Type_Code_ExecuteRead 0x00000A00
#define KeWriteGDT_Type_Code_ExecuteReadAccessed 0x00000B00
#define KeWriteGDT_Type_Code_ExecuteOnlyConforming 0x00000C00
#define KeWriteGDT_Type_Code_ExecuteOnlyConformingAccessed 0x00000D00
#define KeWriteGDT_Type_Code_ExecuteReadConforming 0x00000E00
#define KeWriteGDT_Type_Code_ExecuteReadConformingAccessed 0x00000F00
#define KeWriteGDT_Type_System_Reserved1 0x00000000
#define KeWriteGDT_Type_System_16BitsTssAvailable 0x00000100
#define KeWriteGDT_Type_System_LDT 0x00000200
#define KeWriteGDT_Type_System_16BitsTssBusy 0x00000300
#define KeWriteGDT_Type_System_16BitsCallGate 0x00000400
#define KeWriteGDT_Type_System_TaskGate 0x00000500
#define KeWriteGDT_Type_System_16BitsInterruptGate 0x00000600
#define KeWriteGDT_Type_System_16BitsTrapGate 0x00000700
#define KeWriteGDT_Type_System_Reserved2 0x00000800
#define KeWriteGDT_Type_System_32BitsTSSAvailable 0x00000900
#define KeWriteGDT_Type_System_Reserved3 0x00000A00
#define KeWriteGDT_Type_System_32BitsTSSBusy 0x00000B00
#define KeWriteGDT_Type_System_32BitsCallGate 0x00000C00
#define KeWriteGDT_Type_System_Reserved4 0x00000D00
#define KeWriteGDT_Type_System_32BitsInterruptGate 0x00000E00
#define KeWriteGDT_Type_System_32BitsTrapGate 0x00000F00

//KeWriteIDT
#define KeWriteIDT_P_0 0x0
#define KeWriteIDT_P_1 0x00008000
#define KeWriteIDT_DPL_0 0x0
#define KeWriteIDT_DPL_1 0x00002000
#define KeWriteIDT_DPL_2 0x00004000
#define KeWriteIDT_DPL_3 0x00006000
#define KeWriteIDT_D_0 0x0
#define KeWriteIDT_D_1 2048
#define KeWriteIDT_GateType_TaskGate 256
#define KeWriteIDT_GateType_InterruptGate 1536
#define KeWriteIDT_GateType_TrapGate 1792


//PORT
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21
#define INT_S_CTL 0xA0
#define INT_S_CTLMASK 0xA1
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

#define	RPL_KRNL 0
#define	RPL_TASK 1
#define	RPL_USER 3

#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10
#endif