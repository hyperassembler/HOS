#ifndef __KAHALU_GLOBALS_HEADER_INC
#define __KAHALU_GLOBALS_HEADER_INC

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#ifdef MCU_IP_VERIF
#include <svdpi.h>
#endif //MCU_IP_VERIF

#include <kahalu_global_defines.h>

//Global variables

static REGACC_TYPE g_last_regacc_type;

static uint32_t* g_last_regacc_addr;


#endif  //__KAHALU_GLOBALS_HEADER_INC

