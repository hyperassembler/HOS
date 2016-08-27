/*-------------------------------------------------------
 |
 |  main.c
 |
 |  Architecture specific entry point for Bifrost
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
//extern int platform_init();

int main(int argc, char* argv[])
{
    //// Only the linux platform is supported for x86 right now, so call the platform specific init and nothing else
    //platform_init();
    //initGlobals();
    
    UINT32 dwpid = hw_getCoreNum();
    
    // Check if this core is active
    if(hw_activeCoreList[dwpid])
    {
        if(dwpid == bifrostCachedGlobals.tpid)
        {
            testDriver(dwpid);
        }
        else
        {
            testSlave(dwpid);
        }
    }
    
    for(;;){
        asm volatile("hlt");
    }
}

