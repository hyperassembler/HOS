#include <mm/phys.h>
#include <arch/pmap.h>

struct mm_phys_seg {
    mm_paddr start;
    mm_paddr stop;
};

static ATTR_UNUSED struct list_entry _freelist[MM_PHYS_MAX_POOLS][MM_PHYS_ORDER_MAX];

static struct mm_phys_seg _phys_segs[ARCH_PMAP_MAX_PHYS_SEGS];
static usize _phys_segs_sz = 0;

static struct mm_phys_seg _reserve_segs[ARCH_PMAP_MAX_PHYS_SEGS];
static usize _reserve_segs_sz = 0;

static void
_set_phys_seg(struct mm_phys_seg *seg, mm_paddr start, usize len)
{
    seg->start = start;
    seg->stop = start + len - 1;
}

void
mm_phys_add_phys_seg(mm_paddr start, usize len)
{
    KASSERT(_phys_segs_sz < ARCH_PMAP_MAX_PHYS_SEGS, "too many physical segments!");
    _set_phys_seg(&_phys_segs[_phys_segs_sz], start, len);
    _phys_segs_sz++;
}

void
mm_phys_add_reserved_seg(mm_paddr start, usize len)
{
    KASSERT(_reserve_segs_sz < ARCH_PMAP_MAX_PHYS_SEGS, "too many reserved segments!");
    _set_phys_seg(&_reserve_segs[_reserve_segs_sz], start, len);
    _reserve_segs_sz++;
}
