/*-----------------------------------------------------
 |
 |      hw_rand.c
 |
 |
 | See Knuth vol 2 section 3.2.2
 | for a discussion of this random number generator.
 |
 |------------------------------------------------------
 |
 |  Revision History :
 |
 |    #7 - 15 February 2005 J. Hanes
 |         Drop hw_api_trace_enable; use SHARED_ERRMSG, SHARED_TRACE
 |
 |    #6 -  2 August 2004   J. Hanes
 |          Add missing hw_rand trace when hw_api_trace_enable
 |
 |    #5 -  26 May 2004     J. Hanes
 |          Can't use dynamic allocation of rand state
 |          in bringup code, so make it static for everyone.
 |
 |    #4 -  10 May 2004     J. Hanes
 |          Fix bug -- have to alloc state if hw_srand gets called first.
 |          Add debug prints
 |
 |    #3 -  31 March 2004   J. Hanes
 |          Expose srand in hw_ API.
 |
 |    #2 -  23 March 2004   J. Hanes
 |          Give each test its own private state so that
 |            it will get the same sequence whether run
 |            by itself or concurrently.
 |
 |    #1 -  8 February 2004 J. Hanes
 |          Copied/modified from
 |          hwdev/antichips/antinemo/sysver/lib/src/shared/SSW_stdlib/rand.c
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 1997  CagEnt Technologies Inc.
 |    Copyright (C) 1999  WebTV Networks Inc.
 |    Copyright (C) 2004  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */


#include "bifrost_private.h"

//
// Random number generator states.
//
HW_RANDSTATE randStates[HW_PROC_CNT][HW_TS_MAX_TESTS] __attribute__ ((section(".ddr_wb.bss")));

/*
 * Here are the values that are put in the
 * rand state array by a call to private_srand( 0xffff0002 ).
 *
 *  0x00000001, 0x66d78e85, 0xd5d38c09, 0x0a09d8f5, 0xbf1f87fb,
 *  0xcb8df767, 0xbdf70769, 0x503d1234, 0x7f4f84c8, 0x61de02a3,
 *  0xa7408dae, 0x7a24bde8, 0x5115a2ea, 0xbbe62e57, 0xf6d57fff,
 *  0x632a837a, 0x13861d77, 0xe19f2e7c, 0x695f5705, 0x87936b2e,
 *  0x50a19a6e, 0x728b0e94, 0xc5cc55ae, 0xb10a8ab1, 0x856f72d7,
 *  0xd0225c17, 0x51c4fda3, 0x89ed9861, 0xf1db829f, 0xbcfbc59d,
 *  0x83eec189, 0x6359b159, 0xcc505c30, 0x9cbc5ac9, 0x2fe230f9,
 *  0x39f65e42, 0x75157bd2, 0x40c158fb, 0x27eb9a3e, 0xc582a2d9,
 *  0x0569d6c2, 0xed8e30b3, 0x1083ddd2, 0x1f1da441, 0x5660e215,
 *  0x04f32fc5, 0xe18eef99, 0x4a593208, 0x5b7bed4c, 0x8102fc40,
 *  0x515341d9, 0xacff3dfa, 0x6d096cb5, 0x2bb3cc1d, 0x253d15ff
 *
 */

static void
private_srand( HW_RANDSTATE* prs_p, UINT32 seed )
{
    prs_p->rand_j = 23;
    prs_p->rand_k = 54;
    prs_p->initialSeed = seed;
    prs_p->currentIteration = 0;

    int ii;
    for ( ii = 0; ii < 55; ii ++ )
    {
        prs_p->seed_array[ ii ] = seed + (seed >> 16);
        seed = 69069 * seed + 1725307361;
    }
}

static void init_randState(HW_RANDSTATE* prs_p)
{
    UINT32 suiteSeed;
    suiteSeed = hw_read32(&hw_pTestConfigs->suite_rand_seed);
    UINT32 testId = hw_getMyInstanceID();
    UINT32 combinedSeed = (testId * 10000) + suiteSeed;

    hw_debug("Test %u: Default hw_rand seed %u (based on suite seed %u, master seed %u)\n", testId, combinedSeed, suiteSeed, hw_read32(&hw_pTestConfigs->seed));

    private_srand( prs_p, combinedSeed );
    prs_p->isInitialized = TRUE;
}

INT32 hw_rand()
{
    UINT32 temp;
    HW_RANDSTATE* prs_p;
#if defined(__XTENSA__)
    __asm__ __volatile__ (
        "l32i a3, %[dwCurrentXTest], 0 \n"
        "mull a3, a3, %[randStateSize] \n"
        "add %[prs_p], a3, %[randStates] \n"
        : [prs_p] "=r" ( prs_p )
        : [dwCurrentXTest] "r" ( &testSlaveVars->dwCurrentXTest ), [randStates] "r" ( &randStates[hw_getCoreNum()] ), [randStateSize] "r" ( sizeof(HW_RANDSTATE) )
        : "a3"
    );
#else
    UINT32 curTestIdx = testSlaveVars->dwCurrentXTest;
    prs_p = &randStates[hw_getCoreNum()][curTestIdx];
#endif

    if(!prs_p->isInitialized)
    {
        init_randState(prs_p);
    }

#if defined(__XTENSA__)
    __asm__ __volatile__ (
        "l32i a3, %[rand_j], 0 \n"
        "l32i a4, %[rand_k], 0 \n"
        "addx4 a5, a3, %[seed_array] \n"
        "addx4 a6, a4, %[seed_array] \n"
        "l32i a7, a5, 0 \n"
        "l32i a8, a6, 0 \n"
        "add %[temp], a7, a8 \n"
        "s32i %[temp], a6, 0 \n"
        "movi a5, 54 \n"
        "addi a3, a3, -1 \n"
        "addi a4, a4, -1 \n"
        "movltz a3, a5, a3 \n"
        "movltz a4, a5, a4 \n"
        "s32i a3, %[rand_j], 0 \n"
        "s32i a4, %[rand_k], 0 \n"
        : [temp] "=r" ( temp )
        : [seed_array] "r" ( &prs_p->seed_array ), [rand_j] "r" ( &prs_p->rand_j ), [rand_k] "r" ( &prs_p->rand_k )
        : "a3", "a4", "a5", "a6", "a7", "a8", "memory"
    );
#else
    temp = prs_p->seed_array[ prs_p->rand_j ] + prs_p->seed_array[ prs_p->rand_k ];

    prs_p->seed_array[ prs_p->rand_k ] = temp;

    prs_p->rand_j--;
    if ( prs_p->rand_j < 0 )
    {
        prs_p->rand_j = 54;
        prs_p->rand_k--;
    }
    else
    {
        prs_p->rand_k--;
        if ( prs_p->rand_k < 0 )
        {
            prs_p->rand_k = 54;
        }
    }
#endif
    prs_p->currentIteration += 1;

    temp &= 0x7fffffff;  // result is a 31-bit value

    return (INT32)temp;
}  //  hw_rand()

FLOAT32 hw_randF()
{
    return ((FLOAT32)hw_rand()) / INT_MAX;
}

FLOAT32 hw_distUniform(FLOAT32 low, FLOAT32 high)
{
    return (hw_randF() * (high - low)) + low;
}


void
hw_srandWithIteration( UINT32 seed, UINT32 iteration )
{
    UINT32 curTestIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 testId = hw_getMyInstanceID();

    HW_RANDSTATE* prs_p;

    prs_p = &randStates[hw_getCoreNum()][curTestIdx];

    hw_debug("Test %u: Override hw_rand seed %u starting at iteration %u\n", testId, seed, iteration);

    private_srand( prs_p, seed );
    prs_p->isInitialized = TRUE;

    UINT32 i;
    for(i = 0; i < iteration; i++)
    {
        hw_rand();
    }
}

UINT32 hw_getRandCurrentSeed()
{
    UINT32 curTestIdx = hw_read32(&testSlaveVars->dwCurrentXTest);

    HW_RANDSTATE* prs_p;

    prs_p = &randStates[hw_getCoreNum()][curTestIdx];

    if(!prs_p->isInitialized)
    {
        init_randState(prs_p);
    }

    return prs_p->initialSeed;
}

UINT32 hw_getRandCurrentIteration()
{
    UINT32 curTestIdx = hw_read32(&testSlaveVars->dwCurrentXTest);

    HW_RANDSTATE* prs_p;

    prs_p = &randStates[hw_getCoreNum()][curTestIdx];

    if(!prs_p->isInitialized)
    {
        init_randState(prs_p);
    }

    return prs_p->currentIteration;
}

// Returns random number (r) where: min <= r <= max
INT32 hw_randrange(INT32 min, INT32 max)
{
    return hw_rand() % (max - min + 1) + min;
}
