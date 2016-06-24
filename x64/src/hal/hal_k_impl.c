#include "hal_arch.h"
#include "s_atomic.h"
#include "s_boot.h"
#include "s_context.h"
#include "s_intr.h"
#include "s_vmm.h"

uint64_t KAPI k_interlocked_exchange(uint64_t* target, uint64_t val)
{
    return hal_interlocked_exchange(target, val);
}