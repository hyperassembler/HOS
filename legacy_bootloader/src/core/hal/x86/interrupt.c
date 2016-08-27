/*-------------------------------------------------------
 |
 |    interrupt.c
 |
 |    Fucntions for interrupt handling on 'X86' architecture.
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

void arch_int_globalInit()
{
}

void arch_int_init()
{
}

HW_INT_HANDLER_FUNC* arch_int_registerHandler(HW_INT_VECTOR interrupt, HW_INT_HANDLER_FUNC* handler)
{
    return NULL;
}

UINT32 arch_int_enable(UINT32 mask)
{
    return mask;
}

UINT32 arch_int_disable(UINT32 mask)
{
    return mask;
}

void arch_int_set(UINT32 mask)
{
}

void arch_int_clear(UINT32 mask)
{
}

HW_RESULT arch_int_timerSetTimeout(HW_INT_VECTOR timer, UINT32 timeout)
{
    return HW_E_NOTIMPL;
}

HW_RESULT arch_int_timerSetCompare(HW_INT_VECTOR timer, UINT32 compare)
{
    return HW_E_NOTIMPL;
}

UINT32 arch_int_queryInterrupt(void)
{
    return 0;
}

UINT32 arch_int_queryEnable(void)
{
    return 0;
}


