/*-------------------------------------------------------
 |
 |  platform_functions.c
 |
 |  Platform specific implementations of functions for ISS
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#include <xtensa/simcall.h>


void plat_terminate()
{    
    //if(dwpid == bifrostCachedGlobals.tpid || hw_read32(&hw_pTestConfigs->bfinit.VPA) || !hw_read32(&hw_pTestConfigs->bfinit.STANDALONE))
    if(1)
    {
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
}

int
plat_puts( const char* buffer, int str_length )
{
    // Special override: force the 'iss' platform to print the
    // using the Verilog monitor in cosimulation (like the 'sim'
    // platform does), since the simcalls don't seem to be
    // working properly.
    //if(hw_read32(&hw_pTestConfigs->bfinit.STANDALONE))
    if(1)
    {
        // Use a simcall to print to terminal
        asm volatile( "movi a2, %[code];" // Command: write
                      "movi a3, 1;"       // File: stdout
                      "mov a4, %[buf];"   // String pointer
                      "mov a5, %[len];"   // String length
                      "simcall;"
        :
        : [code] "i" (SYS_write), [buf] "a" (buffer), [len] "a" (str_length)
        : "a2", "a3", "a4", "a5"
        );
    }
    else
    {
        //UINT32 testID = hw_getMyInstanceID();
        //
        //hw_cacheFlushBuffer((void*)buffer, str_length + 1);
        //hw_hostcmd_puts(buffer, testID);
    }
    
    return 0;
}

