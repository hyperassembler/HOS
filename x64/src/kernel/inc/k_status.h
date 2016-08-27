#ifndef _K_STATUS_H_
#define _K_STATUS_H_

#include "g_type.h"
#include "k_stdlib.h"

//
// 32 bit ints
//
// bits 30 - 31 - Error/Success
// 00 = Success
// 01 = Error
//
// bits 0-14 - Return Code - 32768 in total
// bits 15-29 - Facility 32768 in total
//


typedef uint32_t k_status_t;

#define SX_SUCCESS(status) (((status) >> 30) == 0)
#define SX_RETURN(status) (((status) & ke_bit_field_mask(15,29)) >> 15)
#define SX_FACILITY(status) ((status) & ke_bit_field_mask(0,14))

#define SX_MAKE_STATUS(Severity, Facility, Return) (((Severity) << 30) | ((Facility) << 15) | (Return))

#define SEVERITY_ERROR 0x1
#define SEVERITY_SUCCESS 0x0

#define FACILITY_GENERIC 0

typedef enum
{
    STATUS_SUCCESS = SX_MAKE_STATUS(SEVERITY_SUCCESS, FACILITY_GENERIC, 0),
    
};

#endif