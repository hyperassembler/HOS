/*-------------------------------------------------------
 |
 |     init_helper_common.h
 |
 |     Definitions and structs that are used
 |     by both Bifrost and 2BL
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */


#ifndef __INIT_HELPER_COMMON_H__
#define __INIT_HELPER_COMMON_H__

#define INIT_OPTION_BASE_ADDR      (0x00100200)
#define BOOT_DIAG_BASE_ADDR        (0x00100400)
#define CACHED_BOOT_DIAG_BASE_ADDR (0x20100400)

typedef struct
{
    unsigned long long start;
    unsigned long long end;
}BOOT_DIAG_DRAMC_INIT_TIME;

typedef struct
{
    unsigned int pir;
    unsigned int dtcr;
    unsigned int mr3;
    unsigned int zq0pr;
    unsigned int zq1pr;
    unsigned int aclcdlr;
    unsigned int dtdr[2];
    unsigned int dtedr[2];
    unsigned int dx0lcdlr[2];
    unsigned int dx1lcdlr[2];
    unsigned int dx2lcdlr[2];
    unsigned int dx3lcdlr[2];
    unsigned int dx0bdlr[6];
    unsigned int dx1bdlr[6];
    unsigned int dx2bdlr[6];
    unsigned int dx3bdlr[6];
    unsigned int dx4bdlr[6];
    unsigned int dx5bdlr[6];
    unsigned int dx6bdlr[6];
    unsigned int dx7bdlr[6];
    unsigned int dx8bdlr[6];
    unsigned int dx9bdlr[6];
}BOOT_DIAG_DRAMC_TRAINING_STAT;

typedef struct
{
    unsigned int aclcdlr;
    unsigned int dx0lcdlr1;
    unsigned int dx1lcdlr1;
    unsigned int dx2lcdlr1;
    unsigned int dx3lcdlr1;
}BOOT_DIAG_DRAMC_SHMOO_OVERRIDE;

typedef struct
{
    BOOT_DIAG_DRAMC_INIT_TIME        init_time;
    BOOT_DIAG_DRAMC_TRAINING_STAT    training_stat;
    BOOT_DIAG_DRAMC_SHMOO_OVERRIDE   shmoo_override;
}BOOT_DIAG_DRAMC;

typedef struct
{
    BOOT_DIAG_DRAMC dramc;
}BOOT_DIAG;

typedef struct
{
    unsigned int NO_DRAMC_RESET             :1;
    unsigned int NO_DRAMC_INIT              :1;
    unsigned int DRAMC_INIT_FOR_BASRAM_MODE :1;
    unsigned int FULL_DRAMC_INIT            :1;   
    unsigned int DDR_MODE                   :2;
    unsigned int DDR_SIZE                   :4;
    unsigned int DDR_RANKS                  :2;
    unsigned int FREQUENCY_OPT              :11;
    unsigned int UPCTL_OPT                  :1;
    unsigned int DDR_CONF                   :2;
    unsigned int PHY_OPT                    :2;
    unsigned int DRAMC_INTERLEAVE_MODE      :4;
}DRAMC_INIT_OPT_STD;

typedef struct
{
    unsigned int mr3                  :2;
    unsigned int zq0pr                :2;
    unsigned int zq1pr                :2;
    unsigned int rsvd                 :26;
}DRAMC_INIT_OPT_SHMOO_DS;

typedef struct
{
    unsigned int dx0lcdlr1            :8;
    unsigned int dx1lcdlr1            :8;
    unsigned int dx2lcdlr1            :8;   
    unsigned int dx3lcdlr1            :8;
}DRAMC_INIT_OPT_SHMOO_WDQD;

typedef struct
{
    unsigned int dx0lcdlr1            :8;
    unsigned int dx1lcdlr1            :8;
    unsigned int dx2lcdlr1            :8;   
    unsigned int dx3lcdlr1            :8;
}DRAMC_INIT_OPT_SHMOO_RDQSD;

typedef struct
{
    unsigned int dx0lcdlr1            :8;
    unsigned int dx1lcdlr1            :8;
    unsigned int dx2lcdlr1            :8;   
    unsigned int dx3lcdlr1            :8;
}DRAMC_INIT_OPT_SHMOO_RDQSND;

typedef struct
{
    unsigned int acd                  :8;
    unsigned int rsvd                 :24;
}DRAMC_INIT_OPT_SHMOO_AC;

typedef struct
{
    unsigned int ds                   :1;
    unsigned int wdqd                 :1;
    unsigned int rdqsd                :1;
    unsigned int rdqsnd               :1;
    unsigned int ac                   :1;
    unsigned int rsvd                 :27;
}DRAMC_INIT_OPT_SHMOO_ENABLE_MASK;

typedef struct
{
    DRAMC_INIT_OPT_SHMOO_ENABLE_MASK enable;
    DRAMC_INIT_OPT_SHMOO_DS          ds;
    DRAMC_INIT_OPT_SHMOO_WDQD        wdqd; 
    DRAMC_INIT_OPT_SHMOO_RDQSD       rdqsd;
    DRAMC_INIT_OPT_SHMOO_RDQSND      rdqsnd;
    DRAMC_INIT_OPT_SHMOO_AC          ac;
}DRAMC_INIT_OPT_SHMOO;

typedef struct
{
    DRAMC_INIT_OPT_STD               std;
    DRAMC_INIT_OPT_SHMOO             shmoo;
}DRAMC_INIT_OPT;

typedef struct
{
    unsigned int skip_dram_heap_init;
    unsigned int suite_rand_seed;
    unsigned int verbosity;
    unsigned int runtime;
    unsigned int jtm_interval;
    unsigned int jtm_slope;
    unsigned int jtm_offset;
    unsigned int bock_test;
}BF_INIT_OPT;

typedef struct
{
    DRAMC_INIT_OPT dramc_init_opt;
    BF_INIT_OPT    bf_init_opt;
}INIT_OPTION;

#endif // __INIT_HELPER_COMMON_H__

