#ifndef _MEM_H_
#define _MEM_H_
#include "type.h"
#include "kdef.h"

#define SEG_GRANULARITY (1ull << 55)
#define SEG_LONG (1ull << 53)
#define SEG_DPL_0 (0ull << 45)
#define SEG_DPL_1 (1ull << 45)
#define SEG_DPL_2 (2ull << 45)
#define SEG_DPL_3 (3ull << 45)
#define SEG_PRESENT (1ull << 47)
#define SEG_CODE_DATA (1ull << 44)
#define SEG_TYPE_DATA_RW (2ull << 40)
#define SEG_TYPE_DATA_R (0ull << 40)
#define SEG_TYPE_CODE_X (8ull << 40)
#define SEG_TYPE_CODE_XR (10ull << 40)
#define SEG_TYPE_CODE_XC (12ull << 40)
#define SEG_TYPE_CODE_XRC (14ull << 40)
#define SEG_AVAILABLE (1ull << 52)
#define SEG_32_BITS (1ull << 54)

#define PML4_ENTRY_NUM(mem) ((mem) % (4096ull * 512ull * 512ull * 512ull) == 0ull ? (mem)/(4096ull * 512ull * 512ull * 512ull) : (mem) / (4096ull * 512ull * 512ull * 512ull) + 1ull)
#define PDPT_ENTRY_NUM(mem) ((mem) % (4096ull * 512ull * 512ull) == 0ull ? (mem)/(4096ull * 512ull * 512ull) : (mem) / (4096ull * 512ull * 512ull) + 1ull)
#define PD_ENTRY_NUM(mem) ((mem) % (4096ull*512ull) == 0ull ? (mem)/(4096ull*512ull) : (mem) / (4096ull*512ull) + 1ull)
#define PT_ENTRY_NUM(mem) ((mem) % 4096ull == 0ull ? (mem) / 4096ull : (mem) / 4096ull + 1ull)


typedef struct __attribute__ ((packed))
{
	uint16_t limit;
	uint32_t base;
} gdt_ptr_t;

typedef struct __attribute__ ((packed))
{
    uint16_t limit;
    uint32_t base;
} idt_ptr_t;

void HYPKERNEL32 hk_write_segment_descriptor(void *const gdt, uint32_t const base, uint32_t const limit, uint64_t const attr);
extern void HYPKERNEL32 hk_load_gdt(gdt_ptr_t const * const ptr, uint16_t const sel_code, uint16_t const sel_data);
void HYPKERNEL32 hk_mem_cpy(void* src, void* dst, uint32_t size);
void HYPKERNEL32 hk_mem_move(void* src, void* dst, uint32_t size);
extern int32_t HYPKERNEL32 hk_support_x64(void);
extern void hk_disable_paging(void);
extern void hk_enable_paging(void);
void HYPKERNEL32 hk_mem_set(void* src, int8_t const val,uint32_t size);
#endif
