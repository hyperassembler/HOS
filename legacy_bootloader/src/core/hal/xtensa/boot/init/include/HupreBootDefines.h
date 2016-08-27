/*++

Copyright (c) 2013 Microsoft Corporation. All Rights Reserved.

Module Name:

    HupreBootDefines.h

Abstract:

    HupRE boot status value definitions.

Author:

    Jork Loeser (jloeser) 28-Oct-2013

--*/

#pragma once

//
// Status codes propagated from ASIC to SoC.
//
// While the upper 16 bits carry semantic meaning, the lower 16 bits do not
// and can change anytime.
//
// Upper 16 bits:
//
//  0x0000 - reset state
//  0x0001 - 2BL can be downloaded
//  0x0002 - 3BL can be downloaded
//  0x0003 - 3BL has been started and is ready to proceed with handshake.
//  0x8000 - (or'ed) Error. The remaining bits might contain a more details.
//
typedef enum _HUPRE_BOOT_STATUS
{
    HUPRE_BOOT_STATUS_RESET = 0,    // Set by HW
    HUPRE_BOOT_STATUS_1BL_CORE_UP,
    HUPRE_BOOT_STATUS_1BL_READY_FOR_2BL         = 0x00010000,
    HUPRE_BOOT_STATUS_1BL_PCI_UP,
    HUPRE_BOOT_STATUS_1BL_CACHE_CLEANED,
    HUPRE_BOOT_STATUS_1BL_SEEN_2BL,
    HUPRE_BOOT_STATUS_1BL_DONE,
    HUPRE_BOOT_STATUS_2BL_STARTED,
    HUPRE_BOOT_STATUS_2BL_CACHE_UP,
    HUPRE_BOOT_STATUS_2BL_DDR_UP,
    HUPRE_BOOT_STATUS_2BL_READY_FOR_3BL         = 0x00020000,
    HUPRE_BOOT_STATUS_2BL_MSI_MAPPED,
    HUPRE_BOOT_STATUS_2BL_MSI_SENT,
    HUPRE_BOOT_STATUS_2BL_SEEN_3BL,
    HUPRE_BOOT_STATUS_2BL_FIREWALLS_SET,
    HUPRE_BOOT_STATUS_2BL_DONE,
    HUPRE_BOOT_STATUS_3BL_STARTED               = 0x00030000,
    HUPRE_BOOT_STATUS_3BL_TOPOLOGY_DONE,
    HUPRE_BOOT_STATUS_3BL_INTC_LINES_ENABLED,
    HUPRE_BOOT_STATUS_3BL_MSGPOOL_DONE,
    HUPRE_BOOT_STATUS_3BL_INQUEUES_DONE,
    HUPRE_BOOT_STATUS_3BL_PCIE_CONFIG_DONE      = 0x00040000,
    HUPRE_BOOT_STATUS_3BL_PCIE_HANDSHAKE_DONE,
    HUPRE_BOOT_STATUS_ERROR                     = 0x80000000,
} HUPRE_BOOT_STATUS;

//
// INTC specifics to use for status communication from SoC to ASIC
//
enum
{
    //
    // Interrupt line to use. Same for core0/1.
    //
    HUPRE_BOOT_INTC_LINE            = 0,

    //
    // Bits to use for signal readyness of various boot images.
    //
    HUPRE_BOOT_INTC_BIT_2BL = 0,
    HUPRE_BOOT_INTC_BIT_3BL = 1,
};

