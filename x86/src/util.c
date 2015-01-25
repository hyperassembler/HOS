#include "util.h"
#include "type32.h"
#include "kdef32.h"

int32 HYPKERNEL32 hk_set_bit(void* dst,uint32 bit)
{
	if (dst == NULL)
		return -1;
	int32 quo = bit/32;
	bit = bit%32;
	int32* cDst = (int32*)dst + quo;
	*(int32*)(cDst) |= 1 << bit;
	return 0;
}

int32 HYPKERNEL32 hk_clear_bit(void* dst, uint32 bit)
{
	if (dst == NULL)
		return -1;
	int32 quo = bit / 32;
	bit = bit % 32;
	int32* cDst = (int32*)dst + quo;
	*(int32*)(cDst) &= ~(1 << bit);
	return 0;
}

int32 HYPKERNEL32 hk_get_bit(void* dst, uint32 bit)
{
	if (dst == NULL)
		return -1;
	int32 quo = bit / 32;
	bit = bit % 32;
	int32* cDst = (int32*)dst + quo;
	return *(int32*)(cDst) & (1 << bit);
}

int32 HYPKERNEL32 hk_toggle_bit(void* dst, uint32 bit)
{
	if (dst == NULL)
		return -1;
	int32 quo = bit / 32;
	bit = bit % 32;
	int32* cDst = (int32*)dst + quo;
	*(int32*)(cDst) |= 1 << bit;
	return 0;
}


int32 HYPKERNEL32 hk_memcpy(void* src, void* dst, uint32 size)
{
	if (src == NULL || dst == NULL)
		return -1;
	char* cSrc = (char*)src;
	char* cDst = (char*)dst;
	while (size--)
		*(cDst++) = *(cSrc++);
	return 0;
}

int32 HYPKERNEL32 hk_memmove(void* src, void* dst, uint32 size)
{
	if (src == NULL || dst == NULL)
		return -1;
	char* cSrc = (char*)src;
	char* cDst = (char*)dst;
	if (cSrc >= cDst)
	{
		return hk_memcpy(src,dst,size);
	}
	cSrc += size;
	cDst += size;
	while (size--)
		*(--cDst) = *(--cSrc);
	return 0;
}

int32 HYPKERNEL32 hk_print_string(char* str)
{
	char* gs = (char*)(0xb8000);
	uint8 attr = 0x07;
	while (*(str) != 0)
	{
		*(gs) = *(str);
		str++;
		*(gs + 1) = attr; 
		gs += 2;
	}
	return 0;
}

int32 hk_print_int(int32 number)
{
	char arr[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int32 index = 10;
	int32 isNegative = 0;
	int32 div = 10;
	if (number < 0)
	{
		isNegative = 1;
		number *= -1;
	}
	while (1)
	{
		int32 quo = number / div;
		int32 rmd = number%div;
		number = quo;
		arr[index] = '0' + rmd;
		index--;
		if (number == 0)
			break;
	}
	if (isNegative)
	{
		arr[index] = '-';
	}
	hk_print_string(&(arr[index]));
	return 0;
}

int32 HkUpdateCursor(int32 row, int32 col)
{
	if (row < 0 || col < 0 || row > 25 || col > 80)
		return -1;
	uint16 position = (row * 80) + col;
	// cursor LOW port to vga INDEX register
	hk_write_port(0x3D4, 0x0F);
	hk_write_port(0x3D5, (uint8)(position & 0xFF));
	// cursor HIGH port to vga INDEX register
	hk_write_port(0x3D4, 0x0E);
	hk_write_port(0x3D5, (uint8)((position >> 8) & 0xFF));
	return 0;
}