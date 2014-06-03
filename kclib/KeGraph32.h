#ifndef _KeGraph32_h_
#define _KeGraph32_h_
#include "KeDef.h"
#include "KeGlobalVariables.h"
#define DOSGraphColumn 80
#define DOSGraphRow 25
#define DOSGraphMaxBytes 4000
#define DOSGraphBase 0xb8000

VOID HYPKERNELAPI _asm_KePrintHex32(ULONG32 Hex);
VOID HYPKERNELAPI _asm_KePrintStr32(PCHAR OffsetStr);
ULONG32 HYPKERNELAPI _asm_KeSetGraphPosition32(ULONG32 Row,ULONG32 Column);
VOID HYPKERNELAPI _asm_KeClearGraph32(VOID);
ULONG32 HYPKERNELAPI _asm_KeGraphRowOverflow(ULONG32 GraphPosition);
ULONG32 HYPKERNELAPI KeCheckGraphRowForPrint(ULONG32 GraphPosition);
ULONG32 HYPKERNELAPI KeCheckGraphRowForReturn(ULONG32 GraphPosition);
ULONG32 HYPKERNELAPI  _asm_KeGetCurrentGraphPosition32(VOID);
ULONG32 HYPKERNELAPI KeGetCurrentGraphRow(ULONG32 GraphPosition);
#endif
