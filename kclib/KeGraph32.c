#include "KeGraph32.h"
#include "KeGlobalVariables.h"
ULONG32 KeGetCurrentGraphRow( _IN_ ULONG32 GraphPosition)
{
	return (GraphPosition/2*DOSGraphColumn);
}

ULONG32 KeCheckGraphRowForPrint(_IN_ ULONG32 GraphPosition)
{
	if(GraphPosition>=DOSGraphMaxBytes)
	{
		GraphPosition = _asm_KeGraphRowOverflow(GraphPosition);
	}
	return GraphPosition;
}

ULONG32 KeCheckGraphRowForReturn(_IN_ ULONG32 GraphPosition)
{
	ULONG32 GraphRow = GraphPosition/(2*DOSGraphColumn);
	if(GraphRow >= (DOSGraphRow-1))
	{
		GraphPosition = _asm_KeGraphRowOverflow(GraphPosition);
		GraphPosition = _asm_KeSetGraphPosition32(23,0);
	}
	return GraphPosition;
}
