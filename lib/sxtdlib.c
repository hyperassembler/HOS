#include "type.h"
#include "lib/sxtdlib.h"

void SXAPI lb_mem_copy(void *src, void *dst, uint64 size)
{
	if (src == NULL || dst == NULL)
	{
		return;
	}
	char *cSrc = (char *) src;
	char *cDst = (char *) dst;
	while (size--)
	{
		*(cDst++) = *(cSrc++);
	}
}

void SXAPI lb_mem_set(void *src, uint8 const val, uint64 size)
{
	if (src == NULL)
	{
		return;
	}
	while (size--)
	{
		*(uint8 *) src = val;
		src = (void *) ((uintptr) src + 1);
	}
}

void SXAPI lb_mem_move(void *src, void *dst, uint64 size)
{
	if (src == NULL || dst == NULL)
	{
		return;
	}
	if (src >= dst)
	{
		lb_mem_copy(src, dst, size);
		return;
	}
	src = (void *) ((uintptr) src + size - 1);
	dst = (void *) ((uintptr) dst + size - 1);
	while (size--)
	{
		*(char *) dst = *(char *) src;
		dst = (void *) ((uintptr) dst - 1);
		src = (void *) ((uintptr) src - 1);
	}
}

//
// Random Generator
//
static uint32 seed = 1;
static uint32 max = 16777215;

uint32 SXAPI lb_rand(void)
{
	seed = seed * 1103512986 + 29865;
	return (unsigned int) (seed / 65536) % (max + 1);
}

void SXAPI lb_srand(uint32 _seed)
{
	seed = _seed;
}

void SXAPI lb_mrand(uint32 _max)
{
	max = _max;
}

//
// String Library
//

uint64 SXAPI lb_str_len(char const *str)
{
	uint64 length = 0;
	if (str == NULL)
	{
		return 0;
	}
	while (*str != 0)
	{
		str++;
		length++;
	}
	return length;
}

uint64 SXAPI lb_str_cmp(char const *str1, char const *str2)
{
	if (str1 == NULL || str2 == NULL)
	{
		return 0;
	}
	uint64 length = lb_str_len(str1);
	if (length != lb_str_len(str2))
	{
		return 0;
	}
	while (length--)
	{
		if (*(str1 + length) != *(str2 + length))
		{
			return 0;
		}
	}
	return 1;
}
