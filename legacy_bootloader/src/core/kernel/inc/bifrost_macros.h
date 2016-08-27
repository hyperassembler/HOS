/*-------------------------------------------------------
 |
 |      bifrost_macros.h
 |
 |      Contains commonly used macros
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013 Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_MACROS_H
#define _BIFROST_MACROS_H

//
// Some compile macros imply other macros
//
#ifdef BASRAM_HOSTMEM
#define BASRAM_ONLY
#endif

#ifdef BASRAM_ONLY
#define REDUCED_MEMORY_FOOTPRINT
#endif

//
// C-portable version of extern "C"
//
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif

//
// Returns the stringified value of a macro
//
#define _STR(macro) #macro
#define STR(macro) _STR(macro)

//
// Combine two macros or statements
//
#define _CONCAT(x,y) x##y
#define CONCAT(x,y) _CONCAT(x,y)

//
// Test function defines
//
#define INIT_TEST CONCAT(init_, TEST_NAME)
#define RUN_TEST CONCAT(runTest_, TEST_NAME)
#define CLEANUP_TEST CONCAT(cleanup_, TEST_NAME)

//
// Bifrost V3 parameter retrieval
//

#define TEST_PARAMS_CLASS(tpName) \
  class tpName; \
  extern tpName  CONCAT(TestParamsArray__, TEST_NAME)[];

#define TEST_VARS_CLASS(tpName) \
  class tpName; \
  extern tpName  CONCAT(TestVarsArray__, TEST_NAME)[];

#define hw_getTestParamsPtr(void) hw_initTestParams((BifrostTestParams*) hw_getBufferPtr(CONCAT(TestParamsArray__, TEST_NAME)))
#define hw_getTestVarsPtr(void)   hw_initTestVars((BifrostTestVars*) hw_getBufferPtr(CONCAT(TestVarsArray__, TEST_NAME)))

//
// Mimic C++11 static_assert to allow compile-time assertions.
//
// Makes use of the fact that failed assertions will try to redefine the
// dummy typedef to a different array size, which causes a build error.
//
// If you see "error: conflicting declaration" it means your assert does not hold.
//

#if !defined(static_assert)
#define static_assert_plain(val) extern char _static_assert_arr[1]; extern char _static_assert_arr[(val) != 0];
#define static_assert(val, ...) static_assert_plain(val); // For compat with C++ two-parameter version
#endif

//
// Simple macros for rounding an unsigned integer
// up or down to a multiple of some number
//
#define roundUpToMultipleUnsigned(num, multiple) ((multiple) ? (num) + ((num) % (multiple) ? (multiple) - ((num) % (multiple)) : 0) : 0)
#define roundDownToMultipleUnsigned(num, multiple) ((multiple) ? (num) - ((num) % (multiple)) : 0)

//
// Tensilica-specific defines: normal asserts are not functional at the moment.
// In other environments just use normal assert.
//
#ifdef __XTENSA__
#define assert hw_assert
#include <xtensa/config/core.h>
#else
#include <assert.h>
#endif

//
// Macro that returns number of elements in
// an array.
//
#if !defined(__cplusplus)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
extern "C++"
{
template <typename _CountofType, size_t _SizeOfArray>
char (&__countof_helper(_CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(__countof_helper(_Array))
}
#endif

// Macros for using the RDTSCP time stamp counters in x86 environments
#ifdef __X86__
#define RDTSCP() ( \
{ \
UINT64 ret_val; \
asm volatile (".byte 15, 1, 249 \n" \
"shl $0x20,%%rdx \n" \
"or %%rdx,%%rax \n" \
: "=a" (ret_val) \
: \
: "rcx", "rdx"); \
\
ret_val; \
})

#define RDTSC() ( \
{ \
UINT64 ret_val; \
asm volatile (".byte 15, 49 \n" \
"shl $0x20,%%rdx \n" \
"or %%rdx,%%rax \n" \
: "=a" (ret_val) \
: \
: "rdx"); \
ret_val;  \
})
#endif

//
// Macros for bit manipulation
//
// Single bit
#define BIT_SET(data, position) ((data) | (1 << (position)))
#define BIT_CLEAR(data, position) ((data) | ~(1 << position))
#define BIT_FLIP(data, position) ((data) ^ (1 << (position)))
#define BIT_CHECK(data, position) (((data) & (1 << (position))) != 0)
// Bits
#define BITS_SET(data, mask) ((data) | (mask))
#define BITS_CLEAR(data, mask) ((data) & (~(mask)))
#define BITS_FLIP(data, mask) ((data) ^ (mask))
#define BITS_GET(data, mask) ((data) & (mask))

// Target's valid data is right-aligned (bit 0 to bit bit_count-1 contains valid data to set)
// input_value is the number that will be operated on, target contains the bits that will be set at specified location
#define BITS_SET32(input_value, lsb_position, bit_count, target) ((input_value&(~((0xffffffff>>(32 - bit_count))<<lsb_position)))|(target<<lsb_position))
#define BITS_SET64(input_value, lsb_position, bit_count, target) ((input_value&(~((0xffffffffffffffffull>>(64 - bit_count))<<lsb_position)))|(target<<lsb_position))

//Returned data is right-aligned (bit 0 to bit bit_count-1 contains valid data got from input_value)
#define BITS_GET32(input_value, lsb_position, bit_count) ((input_value>>lsb_position)&(0xffffffff>>(32 - bit_count)))
#define BITS_GET64(input_value, lsb_position, bit_count) ((input_value>>lsb_position)&(0xffffffffffffffffull>>(64 - bit_count)))



#define OBTAIN_STRUCT_ADDR(member_addr, member_name, struct_name) ((struct_name*)((char*)(member_addr)-(char*)(&(((struct_name*)0)->member_name))))

#endif
