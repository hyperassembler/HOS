/*-------------------------------------------------------
 |
 |    atomic.c
 |
 |    Atomic operations that use instructions specific to
 |    the x86 ISA
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

void hw_lock(hw_lock_t* lock, const bool yield)
{
    const HW_TESTID id = hw_getMyInstanceID();
    while (HW_TESTID(-1) != hw_storeConditional(&lock->owner, HW_TESTID(-1), id))
    {
        // TODO: yield?
    }
}

void hw_unlock(hw_lock_t* lock, const bool yield)
{
    hw_assert(hw_getMyInstanceID() == lock->owner);
    const_cast<volatile HW_TESTID*>(&lock->owner)[0] = HW_TESTID(-1);
}

