/*-------------------------------------------------------
 |
 |  bifrost_logging.h
 |
 |  Bifrost event log.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2012  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_LOGGING_H
#define _BIFROST_LOGGING_H

// 
// Event log data structures
//

typedef enum {
    HW_EVENT_FAILED_TEST,
    HW_EVENT_FAILED_RESULT,
    HW_EVENT_INT3_BREAKPOINT,
    HW_EVENT_UNHANDLED_INTERRUPT,
    HW_EVENT_UNHANDLED_EXCEPTION,
    HW_EVENT_LOG_FULL,
    HW_EVENT_CHECKPOINT,
    HW_EVENT_TRACEPOINT
} HW_EVENT_CODE;

static HW_PRINT_TYPE event_verbosity_levels[] = {
    HW_CRITICAL,   // HW_EVENT_FAILED_TEST
    HW_CRITICAL,   // HW_EVENT_FAILED_RESULT
    HW_CRITICAL,   // HW_EVENT_INT3_BREAKPOINT
    HW_CRITICAL,   // HW_EVENT_UNHANDLED_INTERRUPT
    HW_CRITICAL,   // HW_EVENT_UNHANDLED_EXCEPTION
    HW_CRITICAL,   // HW_EVENT_LOG_FULL
    HW_CHECKPOINT, // HW_EVENT_CHECKPOINT
    HW_TRACEPOINT, // HW_EVENT_TRACEPOINT
};

typedef struct {
    UINT64 code;
    UINT64 time;
    UINT64 data[HW_MAX_EVENT_LOG_DATA];
} __attribute__((packed)) HW_EVENT_LOG_ENTRY;

typedef struct {
    HW_EVENT_LOG_ENTRY entries[HW_MAX_EVENT_LOG_ENTRIES];
    UINT32 size;
} __attribute__((packed, aligned(HW_MAX_CACHELINE_SIZE))) HW_EVENT_LOG;

//
// Logging API's and macros
//

// The functions used for logging are not supported in C
#ifdef __cplusplus

//
// hw_log, primary logging function
//
// The only required argument is an event code. Additionally, up to four custom data items may be stored.
// You can modify td_print_event_log_entry to control how the data values are displayed in the log.
// 
extern void hw_log(HW_EVENT_CODE code, UINT64 data0 = 0, UINT64 data1 = 0, UINT64 data2 = 0, UINT64 data3 = 0);

//
// Macro for logging HW_RESULT-based function calls
//
// Usage:
//  Wrap code like: hr = hw_doSomething();
//  With:           hr = HR(hw_doSomething()));
// And the event log will record any calls that result in an error code.
//
#define HR(x) hw_log_call_result((x), (UINT64)(__func__), __LINE__)

inline HW_RESULT hw_log_call_result(HW_RESULT hr, UINT64 func, UINT64 line)
{
    if (hr != HW_S_OK)
    {
        hw_log(HW_EVENT_FAILED_RESULT, hr, func, line);
    }

    // Pass-through of the original result
    return hr;
}

#else // ! __cplusplus

#define HR(x) x
//inline HW_RESULT hw_log_call_result(HW_RESULT hr, UINT64 func, UINT64 line)
//{
//    if (hr != HW_S_OK)
//    {
//        hw_log(HW_EVENT_FAILED_RESULT, hr, func, line, 0);
//    }
//
//    // Pass-through of the original result
//    return hr;
//}

extern void hw_log(HW_EVENT_CODE code, UINT64 data0, UINT64 data1, UINT64 data2, UINT64 data3);

#endif // __cplusplus

#endif // _BIFROST_LOGGING_H

