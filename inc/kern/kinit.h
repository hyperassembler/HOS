#pragma once

#include <kern/lds.h>

typedef void (kinitf)(void*);

struct kinit {
    void* args;
    kinitf* func;
    int pri;
    int subsys;
    const char* name;
};

#define KINIT_START (struct kinit **)LDS_START(kinit)
#define KINIT_STOP (struct kinit **)LDS_STOP(kinit)

LDS_DECL(kinit);

#define KINIT_DECL(_name, _subsys, _pri, _func, _args) \
    static const struct kinit _kinit_##name = { \
            .args = _args, \
            .func = _func, \
            .subsys = _subsys, \
            .pri= _pri, \
            .name = #_name \
    }; \
    LDS_ATTR(kinit) ATTR_USED static const struct kinit * _kinit_lds_##name = &_kinit_##name

enum {
    KINIT_SUBSYS_KERN = 0,
    KINIT_SUBSYS_KTEST = 1
};
