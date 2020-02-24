#include <common/cdef.h>
#include <common/libkern.h>
#include <ke/brute.h>
#include <test/ktest.h>

#define MAX_ELE (10)

static int
int_cmp(const void* a, const void* b)
{
    return *(const int*)a - *(const int*)b;
}

static void
arr_assert(const int* a, const int *b, usize sz)
{
    for(usize i = 0; i < sz; i++) {
        KASSERT(a[i] == b[i], "element at %d not equal: %d != %d",(int)i, a[i], b[i]);
    }
}

static void
test_permutation(int *arr, int *exp, int *buf, usize start, usize sz)
{
    if (start == sz) {
        memcpy(buf, arr, sz * sizeof(int));
        qsort(buf, sz, sizeof(int), int_cmp);
        arr_assert(exp, buf, sz);
    } else {
        for (usize i = start; i < sz; i++) {
            memswp(&arr[start], &arr[i], sizeof(int));
            test_permutation(arr, exp, buf, start + 1, sz);
            memswp(&arr[start], &arr[i], sizeof(int));
        }
    }
}

static void
qsort_test(ATTR_UNUSED void *unused)
{
    int arr[MAX_ELE];
    int exp[MAX_ELE];
    int buf[MAX_ELE];

    for(int i = 0; i < MAX_ELE; i++) {
        arr[i] = i;
        exp[i] = i;
    }

    for(int i = 0; i <= MAX_ELE; i++) {
        test_permutation(arr, exp, buf, 0, i);
        memcpy(arr, exp, sizeof(int) * MAX_ELE);
    }
}

KTEST_DECL(qsort, KTEST_SUBSYS_QSORT, qsort_test, NULL);
