#include <test/ktest.h>
#include <kern/cdef.h>
#include <kern/print.h>
#include <kern/kinit.h>

static uint ktest_cases = 0;

static void
ktest_reset()
{
    ktest_cases = 0;
}

void
ktest_begin(const char* name)
{
    kprintf("    Running test %d: %s...", ktest_cases, name);
    ktest_cases++;
}

static void
ktest_main(ATTR_UNUSED void* args)
{
    kprintf("Running ktest test suite...\n\n");
    // run all ktests XXX: we don't care about priority for now as qsort is part of the test
    for(struct ktest **it = KTEST_START; it < KTEST_STOP; it++) {
        ktest_reset();
        kprintf("Testing subsystem %s...\n", (*it)->name);
        (*it)->func((*it)->args);
        kprintf("%d test cases passed.\n\n", ktest_cases);
    }
    kprintf("All tests completed.\n");
}

KINIT_DECL(ktest, KINIT_SUBSYS_KTEST, 0, ktest_main, NULL);
