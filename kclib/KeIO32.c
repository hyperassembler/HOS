#include "KeIO32.h"
#include "KeGraph32.h"
VOID KeWriteGDT(ULONG32 Index,PIAGDT32 Dest, PKeGDTDescriptor32 Src)
{ 
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


VOID KeInitializeKeGDTDescriptor32(PKeGDTDescriptor32 Desc, ULONG32 Base, ULONG32 Limit, ULONG32 G, ULONG32 P, ULONG32 S, ULONG32 Type, ULONG32 DPL, ULONG32 DB, ULONG32 AVL)
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

VOID KeInitializeKeIDTDescriptor32(PKeIDTDescriptor32 Desc,ULONG32 GateType, ULONG32 DPL, ULONG32 D, ULONG32 P, ULONG32 Selector, ULONG32 Offset)
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
	KeGDTDescriptor32 i;

	*((PULONG16)(&KeGDTPtr[0]))= GDT_DESCRIPTOR_NUMBER * sizeof(IAGDT32)-1;
	*((PULONG32)(&KeGDTPtr[2])) = (ULONG32)&KeGDT[0];

	KeInitializeKeGDTDescriptor32(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Code_ExecuteOnly,KeWriteGDT_DPL_0,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_CODE,&KeGDT[0],&i);

	KeInitializeKeGDTDescriptor32(&i,0x0b8000,0xffff,KeWriteGDT_G_0,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Data_ReadWrite,KeWriteGDT_DPL_3,KeWriteGDT_DB_0,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_GRAPH,&KeGDT[0],&i);

	KeInitializeKeGDTDescriptor32(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Data_ReadWrite,KeWriteGDT_DPL_0,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_DATA,&KeGDT[0],&i);

	KeInitializeKeGDTDescriptor32(&i,0,0,0,0,0,0,0,0,0);
	KeWriteGDT(GDT_INDEX_DUMMY,&KeGDT[0],&i);

	KeInitializeKeGDTDescriptor32(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Code_ExecuteOnly,KeWriteGDT_DPL_3,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_USER_CODE,&KeGDT[0],&i);

	KeInitializeKeGDTDescriptor32(&i,0x0,0xfffff,KeWriteGDT_G_1,KeWriteGDT_P_1,KeWriteGDT_S_1,KeWriteGDT_Type_Data_ReadWrite,KeWriteGDT_DPL_3,KeWriteGDT_DB_1,KeWriteGDT_AVL_0);
	KeWriteGDT(GDT_INDEX_USER_DATA,&KeGDT[0],&i);
}

VOID KeInitInterrupt(VOID)
{
	KeInit8259A();
	KeInitIDT();
}


VOID KeWriteIDT(ULONG32 Index,PIAGATE32 Dest ,PKeIDTDescriptor32 Src)
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
	KeIDTDescriptor32 Desc;
	*(PULONG16)(&KeIDTPtr[0])= IDT_GATE_NUMBER * sizeof(IAGATE32) - 1;
	*(PULONG32)(&KeIDTPtr[2])= (ULONG32)(&KeIDT[0]);
	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeDivideError);
	KeWriteIDT(INT_VECTOR_DIVIDE,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeSingleStepException);
	KeWriteIDT(INT_VECTOR_DEBUG,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeNMI);
	KeWriteIDT(INT_VECTOR_NMI,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeBreakpointException);
	KeWriteIDT(INT_VECTOR_BREAKPOINT,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeOverflow);
	KeWriteIDT(INT_VECTOR_OVERFLOW,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeBoundsCheck);
	KeWriteIDT(INT_VECTOR_BOUNDS,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeInvalidOpcode);
	KeWriteIDT(INT_VECTOR_INVAL_OP,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeCoprNotAvailable);
	KeWriteIDT(INT_VECTOR_COPROC_NOT,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeCoprError);
	KeWriteIDT(INT_VECTOR_COPROC_ERR,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeCoprSegmentOverrun);
	KeWriteIDT(INT_VECTOR_COPROC_SEG,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeDoubleFault);
	KeWriteIDT(INT_VECTOR_DOUBLE_FAULT,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeInvalidTSS);
	KeWriteIDT(INT_VECTOR_INVAL_TSS,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeStackException);
	KeWriteIDT(INT_VECTOR_STACK_FAULT,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeSegmentNotPresent);
	KeWriteIDT(INT_VECTOR_SEG_NOT,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeGeneralProtection);
	KeWriteIDT(INT_VECTOR_PROTECTION,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KePageFault);
	KeWriteIDT(INT_VECTOR_PAGE_FAULT,&KeIDT[0],&Desc);

	//Interrupts
	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt00);
	KeWriteIDT(INT_VECTOR_IRQ0 + 0,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt01);
	KeWriteIDT(INT_VECTOR_IRQ0 + 1,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt02);
	KeWriteIDT(INT_VECTOR_IRQ0 + 2,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt03);
	KeWriteIDT(INT_VECTOR_IRQ0 + 3,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt04);
	KeWriteIDT(INT_VECTOR_IRQ0 + 4,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt05);
	KeWriteIDT(INT_VECTOR_IRQ0 + 5,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt06);
	KeWriteIDT(INT_VECTOR_IRQ0 + 6,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt07);
	KeWriteIDT(INT_VECTOR_IRQ0 + 7,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt08);
	KeWriteIDT(INT_VECTOR_IRQ8 + 0,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt09);
	KeWriteIDT(INT_VECTOR_IRQ8 + 1,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt10);
	KeWriteIDT(INT_VECTOR_IRQ8 + 2,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt11);
	KeWriteIDT(INT_VECTOR_IRQ8 + 3,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt12);
	KeWriteIDT(INT_VECTOR_IRQ8 + 4,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt13);
	KeWriteIDT(INT_VECTOR_IRQ8 + 5,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt14);
	KeWriteIDT(INT_VECTOR_IRQ8 + 6,&KeIDT[0],&Desc);

	KeInitializeKeIDTDescriptor32(&Desc,KeWriteIDT_GateType_InterruptGate,KeWriteIDT_DPL_3,KeWriteIDT_D_1,KeWriteIDT_P_1, GDT_SELECTOR_CODE,(ULONG32)&_asm_KeIrqInt15);
	KeWriteIDT(INT_VECTOR_IRQ8 + 7,&KeIDT[0],&Desc);

	_asm_KeLoadIDT();
}

VOID KeIrqIntHandler(ULONG32 IrqNumber)
{
	_asm_KePrintStr32("\nIrq Interrupt Detected. Irq Number:");
	_asm_KePrintHex32(IrqNumber);
	_asm_KePrintStr32("\n");
}