#include <kern/cdef.h>
#include <kern/brute.h>
#include <kern/kinit.h>
#include <kern/libkern.h>

static int
kinit_cmpf(const void *ki1, const void *ki2)
{
    const struct kinit *const *kinit1 = ki1;
    const struct kinit *const *kinit2 = ki2;

    return (*kinit1)->pri - (*kinit2)->pri;
}

static void
init_kinit()
{
    qsort(KINIT_START, ((uintptr) KINIT_STOP - (uintptr) KINIT_START) / sizeof(struct kinit *), sizeof(struct kinit *),
          kinit_cmpf);
    for (struct kinit **it = KINIT_START; it < KINIT_STOP; it++) {
        (*it)->func((*it)->args);
    }
}

/**
 * Kernel entry point
 * @param boot_info passed by the bootloader
 */
ATTR_UNUSED void KABI
kmain(ATTR_UNUSED void *boot_info)
{
    KASSERT(boot_info != NULL, "bootinfo is NULL");
    init_kinit();
    BRUTE("Control reached end of kmain");
}
