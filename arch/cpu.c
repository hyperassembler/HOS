#include <arch/cpu.h>
#include <ke/cdef.h>

#define GDT_ENTRY_SIZE 8
#define GDT_ENTRY_NUM 9

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


static uint8 _gdts[HAL_CORE_COUNT][GDT_ENTRY_NUM * GDT_ENTRY_SIZE];
static struct hal_gdt_ptr _gdt_ptrs[HAL_CORE_COUNT];

static inline uint32 seg_selector(uint32 index, uint32 rpl)
{
    return (index << 3) + rpl;
}


static void
write_segment_descriptor(void *const gdt, uint32 const base, uint32 const limit, uint64 const attr)
{
    if (gdt == NULL)
    {
        return;
    }
    uint64 const seg_desc = (((uint64) base & 0xFFFF) << 16) | ((((uint64) base >> 16) & 0xFF) << 32) |
                            ((((uint64) base >> 24) & 0xFF) << 56) | ((uint64) limit & 0xFFFF) |
                            ((((uint64) limit >> 16) & 0xF) << 48) | attr;
    ((uint8 *) gdt)[0] = (uint8) (seg_desc & 0xFF);
    ((uint8 *) gdt)[1] = (uint8) ((seg_desc >> 8) & 0xFF);
    ((uint8 *) gdt)[2] = (uint8) ((seg_desc >> 16) & 0xFF);
    ((uint8 *) gdt)[3] = (uint8) ((seg_desc >> 24) & 0xFF);
    ((uint8 *) gdt)[4] = (uint8) ((seg_desc >> 32) & 0xFF);
    ((uint8 *) gdt)[5] = (uint8) ((seg_desc >> 40) & 0xFF);
    ((uint8 *) gdt)[6] = (uint8) ((seg_desc >> 48) & 0xFF);
    ((uint8 *) gdt)[7] = (uint8) ((seg_desc >> 56) & 0xFF);
}

void hal_init_gdt(void)
{
    uint32 coreid = hal_get_core_id();
    // get gdt ready
    hal_write_segment_descriptor((void *) &_gdts[coreid][0], 0, 0, 0);
    hal_write_segment_descriptor((void *) &_gdts[coreid][8], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][16], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &_gdts[coreid][24], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][32], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    hal_write_segment_descriptor((void *) &_gdts[coreid][40], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][48], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &_gdts[coreid][56], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][64], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    _gdt_ptrs[coreid].base = (uint64) &_gdts[coreid];
    _gdt_ptrs[coreid].limit = GDT_ENTRY_NUM * GDT_ENTRY_SIZE - 1;
    hal_flush_gdt(&_gdt_ptrs[coreid], seg_selector(1, 0), seg_selector(2, 0));
}