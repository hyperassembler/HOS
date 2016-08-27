/*-------------------------------------------------------
 |
 |    main.c
 |
 |    Entry point for 'xtensa' architecture.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "conf_defs.h"
#include <xtensa/config/core.h>

char kstack[HW_PROC_CNT][1024*1024*2];

extern int kmain(void);
int main(void)
{
    // Set up stacks first of all
    __asm volatile(
    "mov a1, %[stack]\n"
    :
    : [stack] "r" (kstack[xthal_get_prid()] + sizeof(kstack[0]))
    : "a1"
    );
    
    if(xthal_get_prid() > 3)
    {
        __asm volatile("waiti 15");
    }

    unsigned int wait = 2986 * xthal_get_prid();
    while(wait--);
    
    return kmain();
}


#if 0
#include "bifrost_thread.h"
#include "bifrost_private.h"

extern void system_init();
extern void sys_init_memory();
extern void platform_init();

extern UINTPTR hw_bssInfo[];
extern UINTPTR hw_ctorInfo[];

// Set aside space for kernel stacks
// Align to cacheline boundaries to prevent false
// sharing issues.
UINT8 kernelStack[HW_PROC_CNT][HW_KERNEL_STACK_SIZE] __attribute__ ((aligned (HW_MAX_CACHELINE_SIZE),section(".ddr_wb.bss")));

//
// This variable tracks whether a core should
// start executing Bifrost code or not. The point
// at which the 'stall' occurs, as well as the
// definition of this variable, may change from
// platform to platform.
//
extern volatile UINT8 hw_tsStallFlag[HW_PROC_CNT];

//
// This variable tracks whether memory init is finished
//
MEM_DDR_UC volatile UINT32 hw_memInitFinished = 0;
MEM_DDR_UC volatile UINT32 hw_bssInitFinished = 0;

EXTERN_C void bss_clear_ASM(UINT32 table_start);
EXTERN_C void sram_stall_ASM();
EXTERN_C void do_global_ctors_ASM(UINT32 table_start);
extern HW_RESULT arch_int_startTimer0();
extern void arch_int_timer0_handler(...);

// Can't declare ANY variables in the function
// in which we switch the stack. Otherwise,
// they'll write above the top of our stack.
// So main will just switch the stack and then
// execute its other tasks in a second function
int main_body();
int main()
{
    // Set up stacks first of all
    asm volatile(
        "mov a1, %[stack]\n"
        :
        : [stack] "r" (kernelStack[xthal_get_prid()] + sizeof(kernelStack[0]))
        : "a1"
    );
    
    main_body();
    
    // Should never reach here--this
    // return will NOT work properly
}

extern void testDriver(void* dwpid);
extern void testSlave(void* dwpid);

int main_body()
{
    // memory initialization
    if(hw_getCoreNum() == 0)
    {
        sys_init_memory();
        
        if(hw_pTestConfigs->bfinit.BSS_INIT)
        {
            bss_clear_ASM((UINT32)hw_bssInfo);
        }
        
        hw_bssInitFinished = 1;
        plat_post(BOOT_STATUS_DONE_BSS_INIT, HW_INTERNAL_TESTID);
        
        do_global_ctors_ASM((UINT32)hw_ctorInfo);
        
        plat_post(BOOT_STATUS_DONE_MEM_INIT, HW_INTERNAL_TESTID);
        
        hw_memInitFinished = 1;
    }
    else
    {
        while(hw_bssInitFinished == 0);
        arch_int_startTimer0();
        while(hw_memInitFinished == 0);
    }

    system_init();

    UINT32 dwpid = hw_getCoreNum();

    // Wait for the driver to setup the global IDT
    if(dwpid != hw_read32(&hw_pTestConfigs->tpid))
    {
        while(!hw_bIntrInitFinished);
    }
    ts_exc_init();
    ts_int_init();
    hw_thread_init();
    hw_bIntThreadInitFinished = TRUE;
    
    // Only active cores should run anything    
    if(hw_activeCoreList[dwpid])
    {
        // If there are any PLATFORM SPECIFIC initialization steps,
        // a file should be created under src/bifrost/platform/* that has
        // the code to be executed.  For all other platforms that support
        // this architecture, a STUB function should be created that does
        // the equivalent or nothing at all.
        platform_init();

        UINT32 tid;
        if(dwpid == hw_read32(&hw_pTestConfigs->tpid))
        {
            hw_thread_create(testDriver,
                             (void*)dwpid,
                             PRIORITY_DEFAULT,
                             THREAD_DEFAULT_STACK_SIZE,
                             &tid);
        }
        else
        {
            while(!hw_bIntThreadInitFinished);
            hw_thread_create(testSlave,
                             (void*)dwpid,
                             PRIORITY_DEFAULT,
                             THREAD_DEFAULT_STACK_SIZE,
                             &tid);
        }
        hw_thread_start(tid);
    }

    while(1);
    // Should never reach here--this
    // return will NOT work properly
}

void core_cleanup()
{
    // When the driver finishes, perform a magic write
    // that will end the simulation and communicate
    // pass/fail status.
    if(hw_getCoreNum() == bifrostCachedGlobals.tpid)
    {
        if(td_testDriverGlobalPass)
        {
            hw_write32(&hw_extcomSpace.eot_flag, 0xC001C0DE);
        }
        else
        {
            hw_write32(&hw_extcomSpace.eot_flag, 0xBAADC0DE);
        }
    }

    hw_terminate();
}
#endif

