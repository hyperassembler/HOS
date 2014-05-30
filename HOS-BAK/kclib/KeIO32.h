#include "KeDef.h"
#include "KeGlobalVariables.h"
#include "KeGraph32.h"
#include "KeMemory32.h"
#ifndef _asm_KeIO32_h_
#define _asm_KeIO32_h_
//What e verrrrr
//asdasdasd1
#define	PRIVILEGE_KERNEL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

VOID  _asm_KeWritePort(ULONG16 Port,ULONG8 Value);
ULONG8 _asm_KeReadPort(ULONG16 Port);
VOID _asm_KeLoadIDT(VOID);

VOID KeInit8259A(VOID);
VOID KeInitInterrupt(VOID);
VOID KeSwitchGDT(VOID);
VOID KeInitIDT(VOID);

VOID KeWriteGDT(ULONG32 Index,PGDT_DESCRIPTOR Dest, PWrite_GDT_DESCRIPTOR Src);
VOID KeWriteIDT(ULONG32 Index,PGATE Dest ,PWrite_IDT_DESCRIPTOR Src);

VOID KeExceptionHandler(ULONG32 VectorNumber,ULONG32 ErrorCode,ULONG32 eip,ULONG32 cs,ULONG32 eFlags);
VOID _asm_KeDivideError(VOID);
VOID _asm_KeSingleStepException(VOID);
VOID _asm_KeNMI(VOID);
VOID _asm_KeBreakpointException(VOID);
VOID _asm_KeOverflow(VOID);
VOID _asm_KeBoundsCheck(VOID);
VOID _asm_KeInvalidOpcode(VOID);
VOID _asm_KeCoprNotAvailable(VOID);
VOID _asm_KeDoubleFault(VOID);
VOID _asm_KeCoprSegmentOverrun(VOID);
VOID _asm_KeInvalidTSS(VOID);
VOID _asm_KeSegmentNotPresent(VOID);
VOID _asm_KeStackException(VOID);
VOID _asm_KeGeneralProtection(VOID);
VOID _asm_KePageFault(VOID);
VOID _asm_KeCoprError(VOID);

VOID KeIrqIntHandler(ULONG32 IrqNumber);
VOID _asm_KeIrqInt00(VOID);
VOID _asm_KeIrqInt01(VOID);
VOID _asm_KeIrqInt02(VOID);
VOID _asm_KeIrqInt03(VOID);
VOID _asm_KeIrqInt04(VOID);
VOID _asm_KeIrqInt05(VOID);
VOID _asm_KeIrqInt06(VOID);
VOID _asm_KeIrqInt07(VOID);
VOID _asm_KeIrqInt08(VOID);
VOID _asm_KeIrqInt09(VOID);
VOID _asm_KeIrqInt10(VOID);
VOID _asm_KeIrqInt11(VOID);
VOID _asm_KeIrqInt12(VOID);
VOID _asm_KeIrqInt13(VOID);
VOID _asm_KeIrqInt14(VOID);
VOID _asm_KeIrqInt15(VOID);

VOID KeInitializeWriteGDTDescriptor(PWrite_GDT_DESCRIPTOR Desc, ULONG32 Base, ULONG32 Limit, ULONG32 G, ULONG32 P, ULONG32 S, ULONG32 Type, ULONG32 DPL, ULONG32 DB, ULONG32 AVL);
VOID KeInitializeWriteIDTDescriptor(PWrite_IDT_DESCRIPTOR Desc,ULONG32 GateType, ULONG32 DPL, ULONG32 D, ULONG32 P, ULONG32 Selector, ULONG32 Offset);


VOID KeWriteGDT(ULONG32 Index,PGDT_DESCRIPTOR Dest, PWrite_GDT_DESCRIPTOR Src)
{ //To be test by the previous one
	ULONG32 temp = 0;
	ULONG32 datal = 0;
	ULONG32 datah = 0;
	temp = (Src->Base);
	datal = ((temp & 0x0000ffff) << 16 );
	temp = Src->Limit;
	datal = datal + (temp & 0x0000ffff);

	temp = Src->Base;
	datah = (temp & 0xff000000);
	datah = datah + Src->AVL;
	datah = datah + Src->DB;
	datah = datah + Src->G;
	datah = datah + Src->P;
	datah = datah + Src->S;
	datah = datah + Src->DPL;
	datah = datah + Src->Type;
	temp = Src->Limit;
	temp = temp & 0x000f0000;
	datah = datah + temp;
	temp = Src->Base;
	datah = datah + ((temp & 0x00ff0000) >> 16);
	*(PULONG32)(Dest+Index) = datal;
	*((PULONG32)(Dest+Index)+1) = datah;
	return;
}


VOID KeInitializeWriteGDTDescriptor(PWrite_GDT_DESCRIPTOR Desc, ULONG32 Base, ULONG32 Limit, ULONG32 G, ULONG32 P, ULONG32 S, ULONG32 Type, ULONG32 DPL, ULONG32 DB, ULONG32 AVL)
{
    Desc->Base = Base;
    Desc->Limit = Limit;
    Desc->G = G;
    Desc->P = P;
    Desc->S = S;
    Desc->Type = Type;
    Desc->DPL = DPL;
    Desc->DB = DB;
    Desc->AVL = AVL;
    return;
}

VOID KeInitializeWriteIDTDescriptor(PWrite_IDT_DESCRIPTOR Desc,ULONG32 GateType, ULONG32 DPL, ULONG32 D, ULONG32 P, ULONG32 Selector, ULONG32 Offset)
{
    Desc->GateType = GateType;
    Desc->DPL = DPL;
    Desc->D = D;
    Desc->P = P;
    Desc->Selector = Selector;
    Desc->Offset = Offset;
    return;
}

VOID KeSwitchGDT(VOID)
{
	Write_GDT_DESCRIPTOR i;

	*((PULONG16)(&KeGDTPtr[0]))= GDT_DESCRIPTOR_NUMBER * sizeof(GDT_DESCRIPTOR)-1;
	*((PULONG32)(&KeGDTPtr[2])) = (ULONG32)&KeGDT[0];

	KeInitializeWriteGDTDescriptor(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Code_ExecuteOnly,KeWriteGDT_DPL_0,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_CODE,&KeGDT[0],&i);

	KeInitializeWriteGDTDescriptor(&i,0x0b8000,0xffff,KeWriteGDT_G_0,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Data_ReadWrite,KeWriteGDT_DPL_3,KeWriteGDT_DB_0,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_GRAPH,&KeGDT[0],&i);

	KeInitializeWriteGDTDescriptor(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Data_ReadWrite,KeWriteGDT_DPL_0,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_DATA,&KeGDT[0],&i);

	KeInitializeWriteGDTDescriptor(&i,0,0,0,0,0,0,0,0,0);
	KeWriteGDT(GDT_INDEX_DUMMY,&KeGDT[0],&i);

	KeInitializeWriteGDTDescriptor(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Code_ExecuteOnly,KeWriteGDT_DPL_3,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_USER_CODE,&KeGDT[0],&i);

	KeInitializeWriteGDTDescriptor(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Data_ReadWrite,KeWriteGDT_DPL_3,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_USER_DATA,&KeGDT[0],&i);
}

VOID KeInitInterrupt(VOID)
{
	KeInit8259A();
	KeInitIDT();
}

VOID KeWriteIDT(ULONG32 Index,PGATE Dest ,PWrite_IDT_DESCRIPTOR Src)
{
        ULONG32 datah = 0;
        ULONG32 datal = 0;
        ULONG32 temp = 0;
        temp = Src->Selector;
        datal = ((temp & 0x0000ffff) << 16);
        temp = Src->Offset;
        datal = datal + (temp & 0x0000ffff);
        
        temp = Src->Offset;
        datah = datah + (temp & 0xffff0000);
        datah = datah + Src->P;
        datah = datah + Src->D;
        datah = datah + Src->DPL;
        datah = datah + Src->GateType;
        
        *(PULONG32)(Dest + Index) = datal;
        *((PULONG32)(Dest + Index)+1) = datah;
        
        return;
}

VOID KeInit8259A(VOID)
{
	_asm_KeWritePort(INT_M_CTL,0x11);
	_asm_KeWritePort(INT_S_CTL,0x11);
	_asm_KeWritePort(INT_M_CTLMASK,INT_VECTOR_IRQ0);
	_asm_KeWritePort(INT_S_CTLMASK,INT_VECTOR_IRQ8);
	_asm_KeWritePort(INT_M_CTLMASK,0x4);
	_asm_KeWritePort(INT_S_CTLMASK,0x2);
	_asm_KeWritePort(INT_M_CTLMASK,0x1);
	_asm_KeWritePort(INT_S_CTLMASK,0x1);
	_asm_KeWritePort(INT_M_CTLMASK,0xFD);
	_asm_KeWritePort(INT_S_CTLMASK,0xFF);
}

VOID KeExceptionHandler(ULONG32 VectorNumber,ULONG32 ErrorCode,ULONG32 eip,ULONG32 cs,ULONG32 eFlags)
{
	_asm_KeClearGraph32();
	_asm_KePrintStr32("Exception Caught:\n");
	_asm_KePrintStr32("VectorNumber:");
	_asm_KePrintHex32(VectorNumber);
	_asm_KePrintStr32("\nErrorCode:");
	_asm_KePrintHex32(ErrorCode);
	_asm_KePrintStr32("\nCS:");
	_asm_KePrintHex32(cs);
	_asm_KePrintStr32("\nEIP:");
	_asm_KePrintHex32(eip);
	_asm_KePrintStr32("\neFlags:");
	_asm_KePrintHex32(eFlags);
}

VOID KeInitIDT(VOID)
{
	*(PULONG16)(&KeIDTPtr[0])= IDT_GATE_NUMBER * sizeof(GATE) - 1;
	*(PULONG32)(&KeIDTPtr[2])= (ULONG32)(&KeIDT[0]);
	Write_IDT_DESCRIPTOR Desc;
    //Exceptions
	KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeDivideError);
	KeWriteIDT(INT_VECTOR_DIVIDE,&KeIDT[0],&Desc);
    
	KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeSingleStepException);
	KeWriteIDT(INT_VECTOR_DEBUG,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeNMI);
	KeWriteIDT(INT_VECTOR_NMI,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeBreakpointException);
	KeWriteIDT(INT_VECTOR_BREAKPOINT,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeOverflow);
	KeWriteIDT(INT_VECTOR_OVERFLOW,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeBoundsCheck);
	KeWriteIDT(INT_VECTOR_BOUNDS,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeInvalidOpcode);
	KeWriteIDT(INT_VECTOR_INVAL_OP,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeCoprNotAvailable);
	KeWriteIDT(INT_VECTOR_COPROC_NOT,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeCoprError);
	KeWriteIDT(INT_VECTOR_COPROC_ERR,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeCoprSegmentOverrun);
	KeWriteIDT(INT_VECTOR_COPROC_SEG,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeDoubleFault);
	KeWriteIDT(INT_VECTOR_DOUBLE_FAULT,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeInvalidTSS);
	KeWriteIDT(INT_VECTOR_INVAL_TSS,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeStackException);
	KeWriteIDT(INT_VECTOR_STACK_FAULT,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeSegmentNotPresent);
	KeWriteIDT(INT_VECTOR_SEG_NOT,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeGeneralProtection);
	KeWriteIDT(INT_VECTOR_PROTECTION,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KePageFault);
	KeWriteIDT(INT_VECTOR_PAGE_FAULT,&KeIDT[0],&Desc);
    
	//Interrupts
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt00);
	KeWriteIDT(INT_VECTOR_IRQ0 + 0,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt01);
	KeWriteIDT(INT_VECTOR_IRQ0 + 1,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt02);
	KeWriteIDT(INT_VECTOR_IRQ0 + 2,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt03);
	KeWriteIDT(INT_VECTOR_IRQ0 + 3,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt04);
	KeWriteIDT(INT_VECTOR_IRQ0 + 4,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt05);
	KeWriteIDT(INT_VECTOR_IRQ0 + 5,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt06);
	KeWriteIDT(INT_VECTOR_IRQ0 + 6,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt07);
	KeWriteIDT(INT_VECTOR_IRQ0 + 7,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt08);
	KeWriteIDT(INT_VECTOR_IRQ8 + 0,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt09);
	KeWriteIDT(INT_VECTOR_IRQ8 + 1,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt10);
	KeWriteIDT(INT_VECTOR_IRQ8 + 2,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt11);
	KeWriteIDT(INT_VECTOR_IRQ8 + 3,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt12);
	KeWriteIDT(INT_VECTOR_IRQ8 + 4,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt13);
	KeWriteIDT(INT_VECTOR_IRQ8 + 5,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt14);
	KeWriteIDT(INT_VECTOR_IRQ8 + 6,&KeIDT[0],&Desc);
    
    KeInitializeWriteIDTDescriptor(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt15);
	KeWriteIDT(INT_VECTOR_IRQ8 + 7,&KeIDT[0],&Desc);
    
	_asm_KeLoadIDT();
}

VOID KeIrqIntHandler(ULONG32 IrqNumber)
{
	_asm_KePrintStr32("\nIrq Interrupt Detected. Irq Number:");
	_asm_KePrintHex32(IrqNumber);
	_asm_KePrintStr32("\n");
}
#endif