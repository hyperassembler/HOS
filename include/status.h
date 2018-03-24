#ifndef STATUS_H
#define STATUS_H

#include "type.h"
#include "lib/sxtdlib.h"

typedef uint32 sx_status;

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

#define SX_MAKE_STATUS(Severity, Facility, Return) ((sx_status)(((Severity) << 30) | ((Facility) << 16) | (Return)))

#define SEVERITY_ERROR 0x3ul
#define SEVERITY_SUCCESS 0x0ul
#define SEVERITY_INFO 0x1ul

#define FACILITY_GENERIC 0ul
#define FACILITY_RF 1ul
#define FACILITY_MM 2ul

static inline bool sx_success(sx_status status)
{
	uint32 severity = status >> 30;
	return (severity == SEVERITY_INFO) || (severity == SEVERITY_SUCCESS);
}

#define STATUS_SUCCESS        (SX_MAKE_STATUS(SEVERITY_SUCCESS, FACILITY_GENERIC, 0))
#define STATUS_FAIL           (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_GENERIC, 0))

#define RF_UNINITIALIZED      (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_RF, 1))
#define RF_ALLOCATION_FAILED  (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_RF, 2))
#define RF_INVALID_ARGUMENTS  (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_RF, 3))
#define RF_INVALID_HANDLE     (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_RF, 4))
#define RF_DUPLICATED_HANDLE  (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_RF, 5))

#define MM_INVALID_ARGUMENTS  (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_MM, 1))
#define MM_ALLOCATION_FAILED  (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_MM, 2))
#define MM_UNINITIALIZED      (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_MM, 3))
#define MM_NOT_ENOUGH_PAGE    (SX_MAKE_STATUS(SEVERITY_ERROR, FACILITY_MM, 4))

#endif
