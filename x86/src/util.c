#include "util.h"
#include "type32.h"
#include "kdef32.h"

INT32 HKA32 HkSetBit32(void* dst,UINT32 bit)
{
	if (dst == NULL)
		return -1;
	INT32 quo = bit/32;
	bit = bit%32;
	INT32* cDst = (INT32*)dst + quo;
	*(INT32*)(cDst) |= 1 << bit;
	return 0;
}

INT32 HKA32 HkClearBit32(void* dst, UINT32 bit)
{
	if (dst == NULL)
		return -1;
	INT32 quo = bit / 32;
	bit = bit % 32;
	INT32* cDst = (INT32*)dst + quo;
	*(INT32*)(cDst) &= ~(1 << bit);
	return 0;
}

INT32 HKA32 HkGetBit32(void* dst, UINT32 bit) 
{
	if (dst == NULL)
		return -1;
	INT32 quo = bit / 32;
	bit = bit % 32;
	INT32* cDst = (INT32*)dst + quo;
	return *(INT32*)(cDst) & (1 << bit);
}

INT32 HKA32 HkToggleBit32(void* dst, UINT32 bit)
{
	if (dst == NULL)
		return -1;
	INT32 quo = bit / 32;
	bit = bit % 32;
	INT32* cDst = (INT32*)dst + quo;
	*(INT32*)(cDst) |= 1 << bit;
	return 0;
}


INT32 HKA32 HkMemcpy(void* src, void* dst, UINT32 size)
{
	if (src == NULL || dst == NULL)
		return -1;
	char* cSrc = (char*)src;
	char* cDst = (char*)dst;
	while (size--)
		*(cDst++) = *(cSrc++);
	return 0;
}

INT32 HKA32 HkMemmove(void* src, void* dst, UINT32 size)
{
	if (src == NULL || dst == NULL)
		return -1;
	char* cSrc = (char*)src;
	char* cDst = (char*)dst;
	if (cSrc >= cDst)
	{
		return HkMemcpy(src,dst,size);
	}
	cSrc += size;
	cDst += size;
	while (size--)
		*(--cDst) = *(--cSrc);
	return 0;
}