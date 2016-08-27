/*-------------------------------------------------------
 |
 |    exception.c
 |
 |    Fucntions for exception handling on 'x86' architecture.
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


void arch_exc_globalInit()
{

}

HW_EXC_HANDLER_FUNC* arch_exc_registerHandler(HW_EXC_VECTOR exception, HW_EXC_HANDLER_FUNC* handler)
{
    return NULL;
}

HW_EXC_VECTOR arch_exc_queryExceptionCause(void)
{
    return (HW_EXC_VECTOR)0;
}
