/*-------------------------------------------------------
 |
 |      bifrost_system_api_x86.h
 |
 |      Contains project-specific Bifrost API
 |      for the 'x86' archtype
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_SYSTEM_API_X86_H
#define _BIFROST_SYSTEM_API_X86_H

#include <bifrost.h>

//
// Caching, atomic support detection based on address
//
static inline BOOL isCachedAddress(UINTPTR addr)
{
    return true;
}

static inline BOOL hw_atomicsSupported(VOID* const)
{
    return true;
}


#if (defined __cplusplus)

template <const unsigned int offset>
static inline UINT32 hw_load_acq(const UINT32* const p)
{
    return ((volatile UINT32*)p)[offset / sizeof(UINT32)];
}

template <const unsigned int offset>
static inline UINT32 hw_load_acq(const INT32* const p)
{
    return ((volatile INT32*)p)[offset / sizeof(INT32)];
}

static inline UINT32 hw_load_acq(const INT32* const p)
{
    return ((volatile INT32*)p)[0];
}

template <const unsigned int offset>
static inline void hw_store_rel(UINT32* const p, const UINT32 v)
{
    ((volatile UINT32*)p)[offset / sizeof(UINT32)] = v;
}

template <const unsigned int offset>
static inline void hw_store_rel(INT32* const p, const INT32 v)
{
    ((volatile INT32*)p)[offset / sizeof(INT32)] = v;
}

static inline void hw_store_rel(INT32* const p, const INT32 v)
{
    ((volatile INT32*)p)[0] = v;
}

#endif // (defined __cplusplus)

static inline UINT32 hw_load_acq(const UINT32* const p)
{
    return ((volatile UINT32*)p)[0];
}

static inline void hw_store_rel(UINT32* const p, const UINT32 v)
{
    ((volatile UINT32*)p)[0] = v;
}

//
// Address translation functions
//
static inline void* hw_uncached(void* const p)
{
    return p;
}

static inline void* hw_cached(void* const p)
{
    return p;
}

#endif // _BIFROST_SYSTEM_API_X86_H

