#include "KeDef.h"
#include "KeGlobalVariables.h"
#ifndef _KeGraph32_h_
#define _KeGraph32_h_

#define DOSGraphColumn 80
#define DOSGraphRow 25
#define DOSGraphMaxBytes 4000
#define DOSGraphBase 0xb8000

VOID _asm_KePrintHex32(ULONG32 Hex);
VOID _asm_KePrintStr32(PCHAR OffsetStr);
ULONG32 _asm_KeSetGraphPosition32(ULONG32 Row,ULONG32 Column);
VOID _asm_KeClearGraph32(VOID);
ULONG32 _asm_KeGraphRowOverflow(ULONG32 GraphPosition);
ULONG32 KeCheckGraphRowForPrint(ULONG32 GraphPosition);
ULONG32 KeCheckGraphRowForReturn(ULONG32 GraphPosition);
ULONG32  _asm_KeGetCurrentGraphPosition32(VOID);
ULONG32 KeGetCurrentGraphRow(ULONG32 GraphPosition);

ULONG32 KeGetCurrentGraphRow(ULONG32 GraphPosition)
{
	return (GraphPosition/2*DOSGraphColumn);
}

ULONG32 KeCheckGraphRowForPrint(ULONG32 GraphPosition)
{
	if(GraphPosition>=DOSGraphMaxBytes)
	{
		GraphPosition = _asm_KeGraphRowOverflow(GraphPosition);
	}
	return GraphPosition;
}

ULONG32 KeCheckGraphRowForReturn(ULONG32 GraphPosition)
{
	ULONG32 GraphRow = GraphPosition/(2*DOSGraphColumn);
	if(GraphRow >= (DOSGraphRow-1))
	{
		GraphPosition = _asm_KeGraphRowOverflow(GraphPosition);
		GraphPosition = _asm_KeSetGraphPosition32(23,0);
	}
	return GraphPosition;
}

#endif
