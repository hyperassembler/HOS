/*-------------------------------------------------------
 |
 |     2bl.c
 |
 |     main of boot loader which is called from reset handler.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "init_helper.h"
#include "HuPPCIeATU.h"
#include "HupreBootDefines.h"
#include "hup_chip.h"
#define BOOT_STACK_SIZE (0x400)
extern int __stack;

BOOT_DIAG* gBootDiagPtr = (BOOT_DIAG*)BOOT_DIAG_BASE_ADDR;
#ifndef PCIE_SYNC
INIT_OPTION gInitOption __attribute__ ((section(".sram_init_opt.data")));
INIT_OPTION* gInitOptionPtr = (INIT_OPTION*)&gInitOption;
#else
INIT_OPTION* gInitOptionPtr = (INIT_OPTION*)INIT_OPTION_BASE_ADDR;
#endif

extern void dramc_init(int dpd);

void
StatusUpdate(
    HUPRE_BOOT_STATUS   Status
    )
/*++

Routine Description:

    Perform a status update the SoC can see to trace our progress.

Arguments:

    Status - Status to write

Return value:

    None.

--*/
{
    reg_write32(HUP_CN_MISC_INTC_SCRATCH_REG0, Status);
}

void
DoDramInit()
/*++

Routine Description:

    Setup DDR.

Arguments:

    None.

Return value:

    None.

--*/
{
    dramc_init(0);
}

void
MapAtu()
/*++

Routine Description:

    Temporary ATU setup.

    ISSUE-2014/04/10-jloeser: Remove once HupDriver.sys ATU setup has been
    verified.

Arguments:

    None.

Return value:

    None.

--*/
{
    UINT32              msiSocHigh;
    UINT32              msiSocLow;

    volatile PCIE_ATU_REGISTERS*    regs;

    regs = (volatile PCIE_ATU_REGISTERS *)(HUP_ADDRESS_pcie + PCIE_ATU_STARTING_BYTE_OFFSET);
    //
    // prepare for writing outbound ATU entry 31
    //
    regs->VIEWPORT.Bits.REGION_DIR = PCIE_ATU_REGION_DIR_OUTBOUND; //0;

    regs->VIEWPORT.Bits.REGION_INDEX = 31;

    regs->LWR_BASE.Raw = XTENSA_PCIE_END - 4096;
    regs->UPPER_BASE = 0;
    regs->LIMIT_ADDR.Raw = XTENSA_PCIE_END - 1;

    //
    // Obtain MSI address: Found at offsets 0x54 (low), 0x58 (high)
    // from PCI config space.
    //
    msiSocLow = *(PUINT32)(HUP_ADDRESS_pcie + 0x54);
    msiSocHigh = *(PUINT32)(HUP_ADDRESS_pcie + 0x58);

    regs->LWR_TARGET_ADDR.Raw = msiSocLow & ~0xFFFUL;
    regs->UPPER_TARGET_ADDR = msiSocHigh;

    regs->REGION_CTRL1.Raw = 0;
    regs->REGION_CTRL3.Raw = 0;
    regs->REGION_CTRL2.Raw = 0;
    regs->REGION_CTRL2.OutboundBits.REGION_EN = 1;
}

void
DoGetMsiDetails(
    PUINT32*    MsiAddress,
    PUINT32     MsiData0
    )
/*++

Routine Description:

    Obtain MSI address and adata.

    The last 4KB of the PCI range have been setup by the SoC to contain the
    the MSI page.

    The 12-bit offset into the PCI range and the data base word are found
    found in the PCIe config space.

Arguments:

    MsiAddress - Upon return, will contain the address to write to to trigger
        an MSI.

    MsiData0 - Upon return, will contain the data to write to trigger MSI 0.

Return value:

    None.

--*/
{
    UINT32              msiSocLow;

    //
    // ISSUE-2014/04/10-jloeser: Remove once HupDriver.sys ATU setup has been
    // verified.
    //
    //MapAtu();

    //
    // Obtain MSI address: Found at offsets 0x54 (low) from PCI config space.
    //
    msiSocLow = *(PUINT32)(HUP_ADDRESS_pcie  + 0x54);

    *MsiAddress = (PUINT32)(XTENSA_PCIE_END - 4096 + (msiSocLow & 0xFFCUL));

    //
    // Obtain MSI data: Found in lower 16 bits at offset 0x5C of PCI config
    // space.
    //
    *MsiData0 = *(PUINT32)(HUP_ADDRESS_pcie + 0x5C) & 0xFFFF;
}


void
DoSendMsi(
    PUINT32 MsiAddress,
    UINT32  MsiData
    )
/*++

Routine Description:

    Signal DDR readiness to SoC by sending it an MSI.

Arguments:

    MsiAddress - Address to write to to send an MSI.

    MsiData - Data to write.

Return value:

    None.

--*/
{
    __asm__ __volatile__ ( "memw \n" );
    *MsiAddress = MsiData;
}




void
DoWaitFor3BL()
/*++

Routine Description:

    Wait for 3BL readyness.

Arguments:

    None.

Return value:

    None.

--*/
{
    UINT32  bits;

    do
    {
        bits = *(volatile UINT32*)(HUP_CN_MISC_INTC_P0_0_DATA);
    } while ((bits & 0x2) == 0);

    //
    // Ack our interrupt at the interrupt controller.
    //
    *(UINT32*)(HUP_CN_MISC_INTC_P0_0_DATA) = bits;
}

#ifdef BASRAM_ONLY

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
        "mov a1, %[stack];"
        :
        : [stack] "r" ((int)&__stack - BOOT_STACK_SIZE * get_prid())
        : "a1"
    );
    
    main_body();
    
    // Bogus return, we should never get here
    return 0;
}
    
int main_body()
{
    if(get_prid() == 0)
    {
#ifdef PCIE_SYNC
        *(r32*)PCIE_RESET_RELEASE_ADDR = PCIE_RESET_RELEASE_DATA;
        while(*(r32*)HUP_CN_MISC_INTC_P0_0_DATA == 0);
        *(r32*)HUP_CN_MISC_INTC_P0_0_DATA = 0;
#endif
        StatusUpdate(HUPRE_BOOT_STATUS_2BL_STARTED);
        *(r32*)HUP_CHIP_POR_CLOCKGATING1_ADDRESS    =   0x303FFFF;
        *(r32*)HUP_CHIP_POR_CLOCKGATING2_ADDRESS    =   0x03FFFFFF;
        *(r32*)HUP_CHIP_POR_CLOCKGATING5_ADDRESS    =   0xFFFFFFFF;
        *(r32*)HUP_CHIP_POR_CLOCKGATING6_ADDRESS    =   0xFFFFFFFF;
        if(gInitOptionPtr->dramc_init_opt.std.DRAMC_INIT_FOR_BASRAM_MODE ||
           get_platform_type() == PLAT_VEL ||
           get_platform_type() == PLAT_PAL)
        {
            gBootDiagPtr->dramc.init_time.start = (unsigned long long )get_ccount() * 2;
            DoDramInit();
            gBootDiagPtr->dramc.init_time.end = (unsigned long long )get_ccount() * 2;
            StatusUpdate(HUPRE_BOOT_STATUS_2BL_DDR_UP);
        }
        StatusUpdate(HUPRE_BOOT_STATUS_2BL_READY_FOR_3BL);
        set_dramc_ready();
        StatusUpdate(HUPRE_BOOT_STATUS_2BL_DONE);
    }
    else if(get_prid() == 1)
    {
        wait_for_dramc_ready();
    }
    
    asm volatile(
        "movi a0, 0x20122000 \n"
        "jx   a0             \n"
    );
    
    // Bogus return, we should never get here
    return 0;
}

#else //!BASRAM_ONLY

// No stack switching on this path
// since only one core executes it
int main()
{
    PUINT32 msiAddress;
    UINT32  msiData0;

    StatusUpdate(HUPRE_BOOT_STATUS_2BL_STARTED);
    *(r32*)HUP_CHIP_POR_CLOCKGATING1_ADDRESS    =   0x303FFFF;
    *(r32*)HUP_CHIP_POR_CLOCKGATING2_ADDRESS    =   0x03FFFFFF;
    *(r32*)HUP_CHIP_POR_CLOCKGATING5_ADDRESS    =   0xFFFFFFFF;
    *(r32*)HUP_CHIP_POR_CLOCKGATING6_ADDRESS    =   0xFFFFFFFF;
    if(gInitOptionPtr->dramc_init_opt.std.NO_DRAMC_INIT == 0)
    {
        gBootDiagPtr->dramc.init_time.start = (unsigned long long )get_ccount() * 2;
        DoDramInit();
        gBootDiagPtr->dramc.init_time.end = (unsigned long long )get_ccount() * 2;
    }
#ifndef VALIDATION_MODE  
    set_dramc_ready();
#else
    StatusUpdate(HUPRE_BOOT_STATUS_2BL_DDR_UP); 
    StatusUpdate(HUPRE_BOOT_STATUS_2BL_READY_FOR_3BL);
    DoGetMsiDetails(&msiAddress, &msiData0);
    StatusUpdate(HUPRE_BOOT_STATUS_2BL_MSI_MAPPED); 
    DoSendMsi(msiAddress, msiData0);
    StatusUpdate(HUPRE_BOOT_STATUS_2BL_MSI_SENT);   
    DoWaitFor3BL();
    StatusUpdate(HUPRE_BOOT_STATUS_2BL_SEEN_3BL);
    StatusUpdate(HUPRE_BOOT_STATUS_2BL_DONE);
#endif //VALIDATION_MODE

    // Invalidate cached stack
    asm volatile(
        "mov a2, %[addr]\n"
        "loopnez %[size], .sram_stack_inv_end\n"
        "dhi a2, 0\n"
        "addi a2, a2, 64\n"
        ".sram_stack_inv_end:\n"
        :
        : [addr] "r" ((int)&__stack - BOOT_STACK_SIZE), [size] "r" (BOOT_STACK_SIZE / XCHAL_DCACHE_LINESIZE)
        : "a2"
    );
    
    // Transfer control via jump table
    asm volatile(
        "movi a0, 0xA1000000 \n"
        "l32i a0, a0, 0      \n"
        "jx   a0             \n"
    );

    // Bogus return, we should never get here
    return 0;
}

#endif //BASRAM_ONLY

