#ifndef _BIFROST_STATUSCODE_H
#define _BIFROST_STATUSCODE_H

#include "bifrost_types.h"
//
//  HW_RESULTs are 16-bit values layed out as follows:
//
//   1 1 1 1 1 1
//   5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+---------------+-------------+
//  |S|  Facility     |    Code     |
//  +-+---------------+-------------+
//
//  where
//
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

//
// Severity values
//
#define HW_RESULT_SEVERITY_SUCCESS    0

#define HW_RESULT_SEVERITY_ERROR      1

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//
#define HW_SUCCESS(hr) (((uint16_t)(hr)) >> 15 == 0)

//
// Return the code
//
#define HW_RESULT_CODE(hr) ((hr) & 0x7F)

//
//  Return the facility
//
#define HW_RESULT_FACILITY(hr)  (((hr) & 0x7F80) >> 7)

//
//  Return the severity
//
#define HW_RESULT_SEVERITY(hr)  (((hr) >> 15) & 0x1)

//
// Create an HW_RESULT value from component pieces
//
#define MAKE_HW_RESULT(sev, fac, code) \
    (((uint16_t)(sev)<<15) | ((uint16_t)(fac)<<7) | ((uint16_t)(code)))

#define HW_RESULT_FACILITY_THREAD 6
#define HW_RESULT_FACILITY_DPC 1
#define HW_RESULT_FACILITY_SEM 2
#define HW_RESULT_FACILITY_REF 3
#define HW_RESULT_FACILITY_APC 4
#define HW_RESULT_FACILITY_EVENT 4
#define HW_RESULT_FACILITY_TIMER 5
#define HW_RESULT_NO_FACILITY 0

typedef enum
{
    STATUS_SUCCESS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_SUCCESS,
                                    HW_RESULT_NO_FACILITY, 0),

    THREAD_STATUS_INVALID_ARGUMENT = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                    HW_RESULT_FACILITY_THREAD, 1),

    THREAD_STATUS_INVALID_STATE = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                         HW_RESULT_FACILITY_THREAD, 2),

    THREAD_STATUS_UNINITIALIZED = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                 HW_RESULT_FACILITY_THREAD, 3),

    THREAD_STATUS_OUT_OF_MEMORY = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                 HW_RESULT_FACILITY_THREAD, 4),

    THREAD_STATUS_ID_OVERFLOW = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                               HW_RESULT_FACILITY_THREAD, 5),


    DPC_STATUS_NOT_ENOUGH_MEM = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                               HW_RESULT_FACILITY_DPC, 1),

    DPC_STATUS_INVALID_ARGUMENTS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                  HW_RESULT_FACILITY_DPC, 2),

    DPC_STATUS_NOT_INITIALIZED = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                HW_RESULT_FACILITY_DPC, 3),

    SEM_STATUS_CANNOT_ALLOCATE_MEM = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                    HW_RESULT_FACILITY_SEM, 1),

    SEM_STATUS_OCCUPIED = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                         HW_RESULT_FACILITY_SEM, 2),

    SEM_STATUS_INVALID_ARGUMENTS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                  HW_RESULT_FACILITY_SEM, 3),

    SEM_STATUS_INVALID_CONTEXT = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                HW_RESULT_FACILITY_SEM, 4),


    REF_STATUS_CANNOT_ALLOCATE_MEM = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                    HW_RESULT_FACILITY_REF, 1),

    REF_STATUS_HANDLE_NOT_FOUND = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                              HW_RESULT_FACILITY_REF, 2),

    REF_STATUS_INVALID_ARGUMENTS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                  HW_RESULT_FACILITY_REF, 3),

    REF_STATUS_HANDLE_DUPLICATE = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                              HW_RESULT_FACILITY_REF, 4),

    REF_STATUS_UNINITIALIZED = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                              HW_RESULT_FACILITY_REF, 5),

    REF_STATUS_REF_FREED = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                               HW_RESULT_FACILITY_REF, 6),

    REF_STATUS_NO_EFFECT = MAKE_HW_RESULT(HW_RESULT_SEVERITY_SUCCESS,
                                          HW_RESULT_FACILITY_REF, 7),




    APC_STATUS_CANNOT_ALLOCATE_MEM = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                    HW_RESULT_FACILITY_APC, 1),


    APC_STATUS_INVALID_ARGUMENTS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                  HW_RESULT_FACILITY_APC, 2),

    APC_STATUS_NOT_INITIALIZED = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                  HW_RESULT_FACILITY_APC, 3),



    EVENT_STATUS_CANNOT_ALLOCATE_MEM = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                      HW_RESULT_FACILITY_EVENT, 1),


    EVENT_STATUS_INVALID_ARGUMENTS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                    HW_RESULT_FACILITY_EVENT, 2),


    TIMER_STATUS_SUCCESS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_SUCCESS,
                                          HW_RESULT_FACILITY_TIMER, 0),

    TIMER_STATUS_CANNOT_ALLOCATE_MEM = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                      HW_RESULT_FACILITY_TIMER, 1),

    TIMER_STATUS_INVALID_ARGUMENTS = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                    HW_RESULT_FACILITY_TIMER, 2),

    TIMER_STATUS_NOT_INITIALIZED = MAKE_HW_RESULT(HW_RESULT_SEVERITY_ERROR,
                                                    HW_RESULT_FACILITY_TIMER, 3),

} hw_result_t;

#endif
