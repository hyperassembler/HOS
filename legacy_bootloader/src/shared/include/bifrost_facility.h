/*-----------------------------------------------------
 |
 |    bifrost_facility.h
 |
 |    Global facility codes used to generate status codes and control
 |    print verbosity
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#pragma once

typedef enum
{
    FACILITY_MIN     = 0x00,    // Inclusive
    FACILITY_INVALID = 0x00,
    
    //
    //  Facility Codes RESERVED
    //
    FACILITY_RESERVED_BASE = 0x01,
    FACILITY_WIN32         = 0x07,
    FACILITY_RESERVED_END  = 0x0F,
    
    //
    //  Facility Codes for Bifrost core
    //
    FACILITY_BIFROST_CORE_BASE       = 0x10,
    FACILITY_BIFROST_CORE_TESTDRIVER = 0x11,
    FACILITY_BIFROST_CORE_TESTSLAVE  = 0x12,
    FACILITY_BIFROST_CORE_BOOT       = 0x13,
    FACILITY_BIFROST_CORE_PERFMON    = 0x14,
    FACILITY_BIFROST_CORE_END        = 0x1F,
    
    //
    //  Facility Codes for Libraries
    //
    FACILITY_LIBRARIES_BASE = 0x20,
    FACILITY_LIB_SAFEQUEUE  = 0x21,
    FACILITY_LIB_CDMA       = 0x22,
    FACILITY_LIB_CHECKSUM   = 0x23,
    FACILITY_LIB_CLASSIFIER = 0x24,
    FACILITY_DRIVER_BASE    = 0x25,
    FACILITY_LIB_INTC       = 0x26,
    FACILITY_LIB_PDMA       = 0x27,
    FACILITY_LIB_STUB       = 0x28,
    FACILITY_LIB_ISP        = 0x29,
    FACILITY_LIB_JBL        = 0x2a,
    FACILITY_LIB_DRAM       = 0x30,
    FACILITY_LIB_DISP	    = 0x38,
    FACILITY_LIB_DISP0_IMP  = 0x39,
    FACILITY_LIB_DISP1_IMP  = 0x3a,
    FACILITY_LIB_DISP0_LSR  = 0x3b,
    FACILITY_LIB_DISP1_LSR  = 0x3c,
    FACILITY_LIB_I2C        = 0x40,
    FACILITY_LIB_SPI        = 0x41,
    FACILITY_LIB_UART       = 0x42,
    FACILITY_LIB_GPIO       = 0x43,
    FACILITY_LIB_MSB        = 0x44,
    FACILITY_LIB_AIO        = 0x46,
    FACILITY_LIB_CFPU       = 0x47,
    FACILITY_LIB_DNN        = 0x48,
    FACILITY_LIBRARIES_END  = 0x5F,
    
    //
    //  Facility Codes for Tests
    //
    FACILITY_TESTS_BASE = 0x60,
    FACILITY_TESTS_EXAMPLE = 0x61,
    FACILITY_FIR_TEST = 0x62, 
    FACILITY_REG_TEST = 0x63,
    FACILITY_NODEMISC_TEST = 0x64,
    FACILITY_STUB_TEST = 0x65,
    FACILITY_MEMORY_TEST = 0x66,
    FACILITY_MEMORYORDERING_TEST = 0x67,
    FACILITY_PCIE_TEST = 0x68,
    FACILITY_PRODUCER_CONSUMER_TEST = 0x69,
    FACILITY_CDMA_TEST = 0x6A,
    FACILITY_CL_TEST = 0x6B,
    FACILITY_MSB_TEST = 0x6C,
    FACILITY_TCON_TEST  = 0x6D,
    FACILITY_PSRO_TEST   = 0x6E,
    FACILITY_DMA_TEST   = 0x6F,
    FACILITY_I2C_TEST   = 0x70,
    FACILITY_SPI_TEST   = 0x71,
    FACILITY_UART_TEST  = 0x72,
    FACILITY_GPIO_TEST  = 0x73,
    FACILITY_JBL_TEST   = 0x74,
    FACILITY_PERF_TEST   = 0x75,
    FACILITY_DRAM_TEST   = 0x76,
    FACILITY_ISP_TEST   = 0x77,
    FACILITY_AIO_TEST   = 0x78,
    FACILITY_CFPU_TEST  = 0x79,
    FACILITY_JTM_TEST   = 0x7A,
    FACILITY_DNN_TEST   = 0x7A,
    FACILITY_PMU_TEST   = 0x7B,
    FACILITY_TESTS_END  = 0xFF,
    
    FACILITY_MAX = 0xFF         // Inclusive
} FACILITY;

#define FACILITY_COUNT                        (FACILITY_MAX-FACILITY_MIN+1)

