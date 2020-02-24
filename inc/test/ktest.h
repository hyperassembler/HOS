#pragma once

#include <common/cdef.h>
#include <common/lds.h>

LDS_DECL(ktest);

typedef void (ktestf)(void*);

struct ktest {
    void* args;
    ktestf* func;
    int subsys;
    const char* name;
};

#define KTEST_START (struct ktest **)LDS_START(ktest)
#define KTEST_STOP (struct ktest **)LDS_STOP(ktest)

#define KTEST_DECL(_name, _subsys, _func, _args) \
    static const struct ktest _ktest_##_name = { \
            .args = _args, \
            .func = _func, \
            .subsys = _subsys, \
            .name = #_name \
    }; \
    LDS_ATTR(ktest) ATTR_USED static const struct ktest * _ktest_lds_##_name = &_ktest_##_name

enum {
    KTEST_SUBSYS_LIST = 0,
    KTEST_SUBSYS_AVL = 1,
    KTEST_SUBSYS_QSORT = 2,
};

void
ktest_begin(const char* name);
