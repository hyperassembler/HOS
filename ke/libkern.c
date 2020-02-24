#include <common/cdef.h>
#include <common/libkern.h>

void
memswp(void *dst, void *src, usize size)
{
    char tmp;
    char *buf1 = dst;
    char *buf2 = src;

    while (size--) {
        tmp = *buf1;
        *buf1 = *buf2;
        *buf2 = tmp;

        buf1++;
        buf2++;
    }
}

void *
memcpy(void *dst, const void *src, usize size)
{
    const char *csrc = (const char *) src;
    char *cdst = (char *) dst;
    while (size--) {
        *(cdst++) = *(csrc++);
    }

    return dst;
}

void *
memset(void *dst, int val, usize size)
{
    while (size--) {
        *(uint8 *) dst = (uchar) val;
        dst = (void *) ((uintptr) dst + 1);
    }

    return dst;
}

void *
memmove(void *dst, const void *src, usize size)
{
    if (src >= dst) {
        memcpy(dst, src, size);
    } else {
        src = (void *) ((uintptr) src + size - 1);
        dst = (void *) ((uintptr) dst + size - 1);
        while (size--) {
            *(char *) dst = *(char *) src;
            dst = (void *) ((uintptr) dst - 1);
            src = (void *) ((uintptr) src - 1);
        }
    }
    return dst;
}

/*
 * randoms taken from FreeBSD
 */

#define NSHUFF (50)

static ulong seed = 937186357;

ulong
krand()
{
    long x, hi, lo, t;

    /*
     * Compute x[n + 1] = (7^5 * x[n]) mod (2^31 - 1).
     * From "Random number generators: good ones are hard to find",
     * Park and Miller, Communications of the ACM, vol. 31, no. 10,
     * October 1988, p. 1195.
     */
    /* Can't be initialized with 0, so use another value. */
    if ((x = seed) == 0)
        x = 123459876;
    hi = x / 127773;
    lo = x % 127773;
    t = 16807 * lo - 2836 * hi;
    if (t < 0)
        t += 0x7fffffff;
    seed = t;
    return (t);
}

void
ksrand(ulong sd)
{
    seed = sd;
    for (int i = 0; i < NSHUFF; i++) {
        krand();
    }
}

/*
 * quicksort
 */
static int
_qsort_partition(void *base, size_t num, size_t sz, int (*cmpf)(const void *, const void *))
{
    void *smaller = base;
    void *pivot = (char *)base + (num - 1) * sz;
    /* number of items smaller than pivot */
    int smaller_idx = 0;

    /* pivot = last element */
    while (base < pivot) {
        if (cmpf(base, pivot) < 0) {
            /* base < pivot */

            /*swap smaller and base*/
            if (smaller != base) {
                memswp(smaller, base, sz);
            }

            smaller_idx++;
            smaller = (char*)smaller + sz;
        }
        base = (char*)base + sz;

    }

    /* swap the pivot to its correct position */
    if (smaller != pivot) {
        memswp(smaller, pivot, sz);
    }

    return smaller_idx;
}

void
qsort(void *base, size_t num, size_t sz, int (*cmpf)(const void *, const void *))
{
    int pivot;
    if (num > 0) {
        pivot = _qsort_partition(base, num, sz, cmpf);

        qsort(base, pivot, sz, cmpf);
        qsort((char*)base + (pivot + 1) * sz, (num - pivot - 1), sz, cmpf);
    }
}
