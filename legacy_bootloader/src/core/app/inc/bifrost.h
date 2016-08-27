#ifndef _BIFROST_H
#define _BIFROST_H

// =======================
// BIFROST User API header
// =======================

// types
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

typedef void (*hw_callback_func_t)(void *kernel_args, void *user_args);

typedef uint32_t hw_handle_t;

#define TRUE  (true)
#define FALSE (false)

// =======================
// Status Codes
// =======================

#define HW_RESULT_SEVERITY_SUCCESS    0
#define HW_RESULT_SEVERITY_ERROR      1

#define HW_SUCCESS(hr) (((uint16_t)(hr)) >> 15 == 0)

#define HW_RESULT_CODE(hr) ((hr) & 0x7F)

#define HW_RESULT_FACILITY(hr)  (((hr) & 0x7F80) >> 7)

#define HW_RESULT_SEVERITY(hr)  (((hr) >> 15) & 0x1)

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

// =======================
// Memory Allocation
// =======================
extern void *hw_alloc(size_t size);

extern void hw_free(void *ptr);

// =======================
// Events
// =======================
typedef enum
{
    EVENT_TYPE_MANUAL,
    EVENT_TYPE_AUTO
} hw_event_type_t;

extern hw_result_t hw_event_wait(hw_handle_t event);

extern hw_result_t hw_event_reset(hw_handle_t event);

extern hw_result_t hw_event_signal(hw_handle_t handle);

extern hw_result_t hw_event_create(hw_handle_t *out, hw_event_type_t event_type);


// =======================
// HAL
// =======================
#define HW_CACHELINE_SIZE (64)

extern uint32_t ke_get_system_tick();

extern void ke_flush_addr(void *addr, uint32_t num_of_cacheline);

extern uint32_t ke_get_current_core();

extern int32_t ke_interlocked_exchange(int32_t *addr, int32_t val);

extern int32_t ke_interlocked_compare_exchange(int32_t *addr, int32_t compare, int32_t val);

extern int32_t ke_interlocked_increment(int32_t *addr, int32_t val);

// =======================
// Print
// =======================
extern void hw_printf(const char *format, ...);


// =======================
// Handles
// =======================
extern hw_result_t hw_close_handle(hw_handle_t handle);


// =======================
// Semaphores
// =======================
extern hw_result_t hw_sem_create(hw_handle_t *out, int32_t count);

extern hw_result_t hw_sem_wait(hw_handle_t handle, int32_t quota);

extern hw_result_t hw_sem_signal(hw_handle_t handle, int32_t quota);

extern hw_result_t hw_sem_trywait(hw_handle_t handle, int32_t quota);

// =======================
// stdlib
// =======================
extern int32_t hw_memcmp(const void *ptr1, const void *ptr2, const size_t len);

extern void hw_memset(void *ptr, uint8_t value, size_t len);

// =======================
// threads
// =======================
#define THREAD_DEFAULT_STACK_SIZE 0x4000

#define THREAD_EXIT_CODE_TERMINATED 0xDEADDEAD

typedef enum
{
    PRIORITY_HIGHEST = 0,
    PRIORITY_HIGH,
    PRIORITY_DEFAULT,
    PRIORITY_LOW,
    PRIORITY_LOWEST,
    PRIORITY_LEVEL_NUM
} hw_thread_priority_t;

extern int32_t hw_current_thread_id();

extern hw_handle_t hw_current_thread();

extern hw_result_t hw_wait_for_thread_exit(hw_handle_t handle);

extern hw_result_t hw_thread_sleep(uint32_t millis);

extern void hw_thread_exit(int32_t exit_code);

extern hw_result_t hw_thread_create(void (*proc)(void *),
                                    void *args,
                                    hw_thread_priority_t priority,
                                    uint32_t stack_size,
                                    hw_handle_t *thread_handle);

extern hw_result_t hw_thread_start(hw_handle_t thread_handle);

extern hw_result_t hw_thread_terminate(hw_handle_t thread_handle);

extern hw_result_t hw_thread_get_exit_code(hw_handle_t thread_handle, int32_t *exit_code);

extern hw_result_t hw_thread_open(int32_t thread_id, hw_handle_t *out);

// =======================
// Timers
// =======================
typedef enum
{
    TIMER_TYPE_MANUAL_RESET,
    TIMER_TYPE_AUTO_RESET
} hw_timer_type_t;

extern hw_result_t hw_timer_create(hw_handle_t *out,
                                   hw_timer_type_t type);

extern hw_result_t hw_timer_wait(hw_handle_t timer_handle);

extern hw_result_t hw_timer_set(hw_handle_t timer_handle, uint32_t tick, bool periodic);

extern hw_result_t hw_timer_cancel(hw_handle_t timer_handle);

#endif
