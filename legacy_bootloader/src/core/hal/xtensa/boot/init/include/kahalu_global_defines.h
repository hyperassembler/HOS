#ifndef __KAHALU_GLOBAL_DEFINES_HEADER_INC
#define __KAHALU_GLOBAL_DEFINES_HEADER_INC

#ifdef MCU_IP_VERIF
#include <stdio.h>
#include <stdlib.h>
#include <svdpi.h>
#include <stddef.h>
#include <unistd.h>
#endif //MCU_IP_VERIF

//Global variables
typedef enum {
    REGACC_RD = 0,
    REGACC_WR = 1
} REGACC_TYPE;

#endif  //__KAHALU_GLOBAL_DEFINES_HEADER_INC

