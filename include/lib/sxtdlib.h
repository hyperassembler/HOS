#ifndef LIB_SXTDLIB_H
#define LIB_SXTDLIB_H

#include "type.h"

uint32 SXAPI lb_rand(void);

void SXAPI lb_srand(uint32 _seed);

void SXAPI lb_mrand(uint32 max);

uint64 SXAPI lb_str_len(char const *str);

uint64 SXAPI lb_str_cmp(char const *str1, char const *str2);

void SXAPI lb_mem_copy(void *src, void *dst, uint64 size);

void SXAPI lb_mem_move(void *src, void *dst, uint64 size);

void SXAPI lb_mem_set(void *src, uint8 const val, uint64 size);

static inline uint64 SXAPI lb_align_down(uint64 val, uint64 alignment)
{
	return (val / alignment) * alignment;
}

static inline uint64 SXAPI lb_align_up(uint64 val, uint64 alignment)
{
	return ((((val) % (alignment)) == 0) ? (((val) / (alignment)) * (alignment)) : (
			(((val) / (alignment)) * (alignment)) + 1));
}

static inline uint64 SXAPI lb_is_overlap(uint64 x1, uint64 x2, uint64 y1, uint64 y2)
{
	return ((x1 <= y2) && (y1 <= x2)) ? 1 : 0;
}


static inline int64 SXAPI lb_max_64(int64 a, int64 b)
{
	return (a) > (b) ? a : b;
}

static inline int64 SXAPI lb_min_64(int64 a, int64 b)
{
	return (a) < (b) ? a : b;
}

static inline int32 SXAPI lb_max_32(int32 a, int32 b)
{
	return (a) > (b) ? a : b;
}

static inline int32 SXAPI lb_min_32(int32 a, int32 b)
{
	return (a) < (b) ? a : b;
}

/*
static inline u64 KAPI round_up_power_of_2(u64 num)
{
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num |= num >> 32;
    num++;
    return (u64)num;
}

static inline uint32 KAPI log_base_2(u64 num)
{
    uint32 result = 0;

    while (num >>= 1)
    {
        result++;
    }

    return result;
}
*/

#define OBTAIN_STRUCT_ADDR(member_addr, struct_name, member_name) ((struct_name*)((uintptr)(member_addr) - (uintptr)(&(((struct_name*)0)->member_name))))

static inline uint64 SXAPI lb_bit_mask(uint32 bit)
{
	return (uint64) 1 << bit;
}

static inline uint64 SXAPI lb_bit_field_mask(uint32 low, uint32 high)
{
	return ~(~(uint64) 0 << high << 1) << low;
}

static inline void SXAPI lb_bit_map_set(void *bit_map, uint64 bit)
{
	if (bit_map != NULL)
	{
		uint64 quot = bit >> 3;
		uint32 rmd = (uint32) (bit & lb_bit_field_mask(0, 2));

		*((uint8 *) (bit_map) + quot) |= (uint8) lb_bit_mask(rmd);
	}
}

static inline void SXAPI lb_bit_map_clear(void *bit_map, uint64 bit)
{
	if (bit_map != NULL)
	{
		uint64 quot = bit >> 3;
		uint32 rmd = (uint32) (bit & lb_bit_field_mask(0, 2));

		*((uint8 *) (bit_map) + quot) &= ~(uint8) lb_bit_mask(rmd);
	}
}

static inline uint32 SXAPI lb_bit_map_read(void *bit_map, uint64 bit)
{
	if (bit_map != NULL)
	{
		uint64 quot = bit >> 3;
		uint32 rmd = (uint32) (bit & lb_bit_field_mask(0, 2));

		return (*((uint8 *) (bit_map) + quot) & (uint8) lb_bit_mask(rmd)) == 0 ? 0 : 1;
	}
	return 0;
}

#endif
