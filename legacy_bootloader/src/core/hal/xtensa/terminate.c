/*-------------------------------------------------------
 |
 |    terminate.c
 |
 |    Fucntions for terminate the execution on a core for 'xtensa' architecture.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

void hw_terminate()
{
    HW_PLATFORM platform = hw_get_platform();
    UINT32 dwpid = hw_getCoreNum();
    
    switch(platform)
    {
    case ISS_PLATFORM:
    case VISS_PLATFORM:
        if(dwpid == bifrostCachedGlobals.tpid || hw_read32(&hw_pTestConfigs->bfinit.VPA) || !hw_read32(&hw_pTestConfigs->bfinit.STANDALONE))
        {
            // In standalone, unconditionally print out a message
            // to announce successful exit of test
            if(dwpid == bifrostCachedGlobals.tpid && hw_read32(&hw_pTestConfigs->bfinit.STANDALONE))
            {
                if(td_testDriverGlobalPass)
                {
                    td_printf("Successful run.\n");
                }
                else
                {
                    td_printf("Unsuccessful run.\n");
                }
            }
            
            // Terminate this core with a simcall
            asm volatile(
                "movi a2, %[code];"
                "simcall;"
                :
                : [code] "i" (SYS_exit)
                : "a2"
            );
        }
        else
        {
            // Do continuous interrupt wait instructions
            // to halt processor execution
            while(1)
            {
                asm volatile("waiti 15");
            }
        }
        break;
    case SIM_PLATFORM:
    case EMU_PLATFORM:
    case CHIP_PLATFORM:
    case RPP_PLATFORM:
        // Do continuous interrupt wait instructions
        // to halt processor execution
        while(1)
        {
            asm volatile("waiti 15");
        }
        break;
    default:
        break;
    }
}
