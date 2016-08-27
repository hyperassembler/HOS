/*-----------------------------------------------------
 |
 |      hw_crc.c
 |
 |------------------------------------------------------
 |
 |    Provides standard IEEE CRC32 functionality.
 |    Adapted from XBox main tree.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2011  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"

#define UINT8_BIT       (sizeof(UINT8) * 8)
#define UINT32_BIT      (sizeof(UINT32) * 8)
#define CRC32_SHIFT_AMT (UINT32_BIT - UINT8_BIT)
#define UINT32_MSB      (1 << (UINT32_BIT - 1))
#define CRCPOLY         (0x04C11DB7) // Standard CRC32 generator polynomial

#if defined(__x86_64__)
#include "emmintrin.h"

// Reflect 16 bytes of an XMM register in parallel
static inline __m128i reflect_data8x16(__m128i data)
{
    // Initialize reflection masks
    const __m128i reflect1 = _mm_set1_epi8(0x55);
    const __m128i reflect2 = _mm_set1_epi8(0x33);
    const __m128i reflect4 = _mm_set1_epi8(0x0F);
    
    // Swap even and odd bits
    data = _mm_or_si128(_mm_and_si128(_mm_srli_epi64(data, 1), reflect1), _mm_slli_epi64(_mm_and_si128(data, reflect1), 1));
    // Swap pairs of bits
    data = _mm_or_si128(_mm_and_si128(_mm_srli_epi64(data, 2), reflect2), _mm_slli_epi64(_mm_and_si128(data, reflect2), 2));
    // Swap groups of 4 bits
    data = _mm_or_si128(_mm_and_si128(_mm_srli_epi64(data, 4), reflect4), _mm_slli_epi64(_mm_and_si128(data, reflect4), 4));
    
    return data;
}

//
// Use SSE 16 byte reads then do the rest of
// the byte-by-byte calculation locally.
//
UINT32 fastcrc32(UINT32 rem, UINT8* ptr, UINT64 sizeInDoubleQuadWords)
{
// The _mm_extract_epi16 intrinsic only takes
// integer constants as an argument (no
// variables). We don't want to repeat this code
// 8 times so make it a macro.
#define processWord(xmm, idx) do { \
        wData = _mm_extract_epi16(xmm, idx); \
        bData = wData; \
        temp = bData ^ (rem >> CRC32_SHIFT_AMT); \
        rem = crc_vals[temp] ^ (rem << 8); \
        bData = wData >> 8; \
        temp = bData ^ (rem >> CRC32_SHIFT_AMT); \
        rem = crc_vals[temp] ^ (rem << 8); \
        } while (0)
    
    UINT8 bData;
    UINT16 wData;
    UINT8 temp;
    
    for(UINT32 dqCtr = 0; dqCtr < sizeInDoubleQuadWords; dqCtr++)
    {
        __m128i dqwData = reflect_data8x16(_mm_loadu_si128((__m128i*)ptr));
        
        processWord(dqwData, 0);
        processWord(dqwData, 1);
        processWord(dqwData, 2);
        processWord(dqwData, 3);
        processWord(dqwData, 4);
        processWord(dqwData, 5);
        processWord(dqwData, 6);
        processWord(dqwData, 7);
        ptr += 16;
    }
    
    return rem;
}
#endif

//
// IEEE CRC32 specification calls for data
// and remainder reflection, which these functions
// will accomplish.
//
UINT8 reflect_data8(UINT8 data)
{
    // Swap even and odd bits
    data = ((data >> 1) & 0x55) | ((data & 0x55) << 1);
    // Swap pairs of bits
    data = ((data >> 2) & 0x33) | ((data & 0x33) << 2);
    // Swap groups of 4 bits
    data = (data >> 4) | (data << 4);
    
    return data;
}

UINT32 reflect_data32(UINT32 data)
{
    // Swap even and odd bits
    data = ((data >> 1) & 0x55555555) | ((data & 0x55555555) << 1);
    // Swap pairs of bits
    data = ((data >> 2) & 0x33333333) | ((data & 0x33333333) << 2);
    // Swap groups of 4 bits
    data = ((data >> 4) & 0x0F0F0F0F) | ((data & 0x0F0F0F0F) << 4);
    // Swap bytes
    data = ((data >> 8) & 0x00FF00FF) | ((data & 0x00FF00FF) << 8);
    // Swap 2-byte pairs
    data = (data >> 16) | (data << 16);
    
    return data;
}


UINT32 hw_crc32(UINT8 *data, SIZE_T sizeInBytes, UINT32 prevRemainder)
{
    UINT32 ctr;
    UINT8* ptr;
    UINT8  temp;
    UINT32 rem = ~reflect_data32(prevRemainder);
    
#if defined(__x86_64__)
    //
    // Use the fastcrc method with the SSE
    // instructions if the datasize is greater
    // than or equal to 16.
    //
    if(sizeInBytes >= 16)
    {
        rem = fastcrc32(rem, data, sizeInBytes / 16);
        
        //
        // In the case of a datasize not divisible by 16,
        // fall back on the old method for the trailing bytes.
        //
        data += 16 * (sizeInBytes / 16);
        sizeInBytes %= 16;
    }
    
#endif
    
    for (ptr = data, ctr = 0;  ctr < sizeInBytes;  ctr++)
    {
        temp = reflect_data8(hw_read8(ptr++)) ^ (rem >> CRC32_SHIFT_AMT);
        rem = crc_vals[temp] ^ (rem << 8);
    }
    
    return ~reflect_data32(rem);
}
