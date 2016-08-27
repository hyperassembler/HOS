#include "bifrost_stdlib.h"

int32_t hw_memcmp(const void *ptr1, const void *ptr2, const size_t len)
{
    size_t ctr = 0;
    uint8_t a = 0;
    uint8_t b = 0;

    for (ctr = 0; (ctr < len) && (a == b); ctr++)
    {
        a = ((uint8_t *) ptr1)[ctr];
        b = ((uint8_t *) ptr2)[ctr];
    }

    if (a < b)
    {
        return -1;
    }
    else if (a > b)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void hw_memcpy(void *destination, const void *source, size_t n)
{
    size_t ctr;

    if((uint64_t)destination % 32 == 0 && (uint64_t)source % 32 == 0)
    {
        for (ctr = 0; ctr < n / sizeof(uint32_t); ctr++)
        {
            *(((uint32_t *) destination) + ctr) = *(((uint32_t *) source) + ctr);
        }

        // If size is not a multiple of sizeof(uint32_t),
        // copy whatever is left
        for (ctr *= sizeof(uint32_t); ctr < n; ctr++)
        {
            *(((uint8_t *) destination) + ctr) = *(((uint8_t *) source) + ctr);
        }
    }
    else
    {
        for (ctr = 0; ctr < n; ctr++)
        {
            *(((uint8_t *) destination) + ctr) = *(((uint8_t *) source) + ctr);
        }
    }
}

void hw_memset(void *ptr, uint8_t value, size_t len)
{
    size_t ctr;
    uint32_t value32 = value;
    value32 |= (value32 << 8) | (value32 << 16) | (value32 << 24);

    //
    // Handle if ptr is not aligned to 4 bytes
    //
    uint8_t *bPtr = (uint8_t *) ptr;

    for (ctr = 0; ctr < len && ctr < (4 - ((uintptr_t) ptr % 4)); ctr++)
    {
        bPtr[ctr] = value;
    }

    //
    // While remaining length is greater than 4 bytes,
    // write 4 bytes at a time
    //
    uint32_t *dPtr = (uint32_t *) (bPtr + ctr);
    len -= ctr;

    for (ctr = 0; ctr < len / 4; ctr++)
    {
        dPtr[ctr] = value32;
    }

    //
    // If the endpoint is not aligned to 4 bytes, handle
    //
    bPtr = (uint8_t * )(dPtr + ctr);
    len -= (ctr * 4);

    for (ctr = 0; ctr < len; ctr++)
    {
        bPtr[ctr] = value;
    }
}

//
//size_t
//hw_strnlen(const char *s, size_t maxlen)
//{
//    static const char *const myname = "hw_strnlen";
//    size_t len = 0;
//
//    if (0 == s)
//    {
//        hw_errmsg("%s: Error: NULL parameter s\n", myname);
//    }
//    while (len < maxlen && s[len])
//    {
//        len++;
//    }
//    if (len >= maxlen)
//    {
//        hw_errmsg("%s: Error: reached buffer length limit %d\n"
//                          "\twithout finding a terminating NULL\n", myname, maxlen);
//    }
//    return len;
//}
//
//
//char *
//hw_strncpy(char *s1, const char *s2, size_t n)
//{
//    size_t ii = 0;
//
//    if (0 == s1)
//    {
//        hw_errmsg("%s: Error: NULL destination parameter s1\n", __func__);
//    }
//    else if (0 == s2)
//    {
//        hw_errmsg("%s: Error: NULL source parameter s2\n", __func__);
//    }
//    else
//    {
//        for (ii = 0; ii < n; ii++)
//        {
//            s1[ii] = s2[ii];
//            if ('\0' == s2[ii])
//            {
//                break;
//            }
//        }
//    }
//    if (ii >= n && n > 0) // if length was non-zero and we reached the end without finding null character
//    {
//        s1[n - 1] = '\0';
//    }
//    return (s1);
//}
//
//
//char *
//hw_strncat(char *s1, const char *s2, size_t n)
//{
//    unsigned int foundnull = 0;
//    size_t sdex;
//    size_t ddex;
//
//    if (0 == s1)
//    {
//        hw_errmsg("%s: Error: NULL destination parameter s1\n", __func__);
//    }
//    else if (0 == s2)
//    {
//        hw_errmsg("%s: Error: NULL source parameter s2\n", __func__);
//    }
//    else
//    {
//        for (sdex = 0, ddex = 0; sdex < n; ddex++)
//        {
//            if (0 == s1[ddex])
//            {
//                foundnull = 1;
//            }
//            if (1 == foundnull)
//            {
//                s1[ddex] = s2[sdex++];
//            }
//        }
//    }
//    if (0 == foundnull)
//    {
//        /*
//         *  ALWAYS terminate the string,
//         *  truncating the output if necessary.
//         */
//        s1[n - 1] = 0;
//        hw_errmsg("%s: Error: reached buffer length limit %d\n"
//                          "\tOutput string truncated\n", __func__, n);
//    }
//    return (s1);
//
//}
//
//
//int32_t
//hw_strcmp(const char *str1, const char *str2)
//{
//    const char *p1 = str1;
//    const char *p2 = str2;
//
//    if ((0 == str1) || (0 == str2))
//    {
//        int rc;
//        hw_errmsg("hw_strcmp: Error: got 0 for input string pointer:  "
//                          "hw_strcmp( 0x%08x, 0x%08x )\n", str1, str2);
//        if (str1 > str2) rc = 1;
//        else if (str1 < str2) rc = -1;
//        else rc = 0;
//        return (rc);
//    }
//    else
//    {
//        while (*p1 && *p2)
//        {
//            if (*p1 > *p2) return (1);
//            else if (*p1 < *p2) return (-1);
//            p1++;
//            p2++;
//        }
//        if (*p1 > *p2) return (1);
//        else if (*p1 < *p2) return (-1);
//        return (0);
//    }
//
//}
//
//UINT64 hw_strhash(const CHAR *str)
//{
//    UINT64 hashval = 5381;
//    uint32_t c = 0;
//
//    while ((c = *str++))
//    {
//        hashval = ((hashval << 5) + hashval) + c; /* hashval * 33 + c */
//    }
//
//    return hashval;
//}
//
//BOOL hw_isspace(UCHAR ch)
//{
//    BOOL ret = FALSE;
//    switch (ch)
//    {
//        case ' ':
//        case '\t':
//        case '\n':
//        case '\v':
//        case '\f':
//        case '\r':
//            ret = TRUE;
//            break;
//        default:
//            break;
//    }
//
//    return ret;
//}
//
//BOOL hw_isupper(UCHAR ch)
//{
//    return (ch >= 'A' && ch <= 'Z');
//}
//
//BOOL hw_islower(UCHAR ch)
//{
//    return (ch >= 'a' && ch <= 'z');
//}
//
//BOOL hw_isdigit(UCHAR ch)
//{
//    return (ch >= '0' && ch <= '9');
//}
//
//
///*-----------------------------------------------------
// |
// |      hw_strtof.c
// |
// |------------------------------------------------------
// |
// |  Revision History :
// |
// |    #2 -  20 January 2005 J. Hanes
// |          pick lint
// |
// |    #1 -  2 March 2004    J. Hanes
// |          Ooops, forgot to p5 add it.
// |       -  8 February 2004 J. Hanes
// |          Copied/modified from
// |          hwdev/antichips/antinemo/sysver/lib/src/shared/SSW_stdlib/strtof.c
// |
// |------------------------------------------------------
// |
// |    Copyright (C) 2004  Microsoft Corporation
// |    All Rights Reserved
// |    Confidential and Proprietary
// |
// |------------------------------------------------------
// */
//
//#define MAX10 (6)
//
//static float tentothe2tothe[MAX10] =
//        {1.0e1, 1.0e2, 1.0e4, 1.0e8, 1.0e16, 1.0e32};
//
//static float
//hw_pow10(int exp)
//{
//    int i = MAX10, j = 32, minus;
//    float f = 1.0;
//
//    if ((minus = (exp < 0)))
//        exp = -exp;
//
//    while (--i >= 0)
//    {
//        if (exp >= j)
//        {
//            f *= tentothe2tothe[i];
//            exp -= j;
//        }
//        j >>= 1;
//    }
//
//    return (minus) ? (1.0 / f) : f;
//}
//
//
//float
//hw_strtof(const char *str,
//          char **endScan)
//{
//    float x = 0.0, div;
//    int negsign, exp = 0, expsign;
//
//    /* eat any leading whitespace */
//    while ((hw_isspace((int) *str)))
//    {
//        str++;
//    }
//
//    if ((negsign = (*str == '-')))
//    {
//        str++;
//    }
//    else if (*str == '+')
//    {
//        str++;
//    }
//
//    /* now read in the first part of the number */
//    while (isdigit((int) *str))
//        x = 10.0 * x + (*str++ - '0');
//
//    /* if we hit a period, do the decimal part now */
//    if (*str == '.')
//    {
//        str++;
//        div = 10.0;
//        while (isdigit((int) *str))
//        {
//            x += (*str++ - '0') / div;
//            div *= 10.0;
//        }
//    }
//
//    /* check for an exponent */
//    if ((*str == 'e') || (*str == 'E'))
//    {
//        str++;
//
//        if ((expsign = (*str == '-')))
//            str++;
//        else if (*str == '+')
//            str++;
//
//        /* handle leading zeros, such as in 1.0e-07 or 1.0e001 */
//        while (*str == '0')
//            str++;
//
//        /* now do the exponent */
//        while (isdigit((int) *str))
//            exp = 10 * exp + (*str++ - '0');
//        if (expsign)
//            exp = -exp;
//
//        if (exp)
//            x *= hw_pow10(exp);
//    }
//
//    if (negsign)
//        x = -x;
//
//    if (endScan)
//        *endScan = (char *) str;
//
//    return x;
//}
//
///*-----------------------------------------------------
// |
// |      hw_strtol.c
// |
// |  hw_strtoul(), hw_strtol(), and hw_atoi()
// |
// |------------------------------------------------------
// |
// |  Revision History :
// |
// |    #3 - 15 February 2005 J. Hanes
// |         Drop hw_api_trace_enable; use SHARED_ERRMSG, SHARED_TRACE
// |
// |    #2 -  28 April 2004   J. Hanes
// |          Check for 0 string pointer in strtol() and strtoul()
// |
// |    #1 -  8 February 2004 J. Hanes
// |          Copied in and modified from
// |          hwdev/antichips/antinemo/sysver/lib/src/shared/SSW_stdlib/strtol.c
// |          Appended atoi() and atol to the bottom.
// |          Replaced NULL with 0
// |
// |------------------------------------------------------
// |
// |    Copyright (C) 1997  CagEnt Technologies Inc.
// |    Copyright (C) 1999  WebTV Networks Inc.
// |    Copyright (C) 2004  Microsoft Corporation
// |    All Rights Reserved
// |    Confidential and Proprietary
// |
// |------------------------------------------------------
// */
//
//
//#define NUMNEG (01000)
//
//
//static int
//_chval(int ch, int radix)
//{
//    int val;
//
//    val = (hw_isdigit(ch) ? (ch) - '0' :
//           hw_islower(ch) ? (ch) - 'a' + 10 :
//           hw_isupper(ch) ? (ch) - 'A' + 10 : -1);
//
//    return (val < radix ? val : -1);
//}
//
//
//static unsigned long int
//_strtoul(const char *nsptr,
//         char **endptr,
//         int base)
//{
//    const unsigned char *nptr = (const unsigned char *) nsptr; /* see scanf */
//    int c, ok = 0;
//
//    while (((c = *nptr++) != 0) && hw_isspace(c));
//    if (c == '0')
//    {
//        ok = 1;
//        c = *nptr++;
//        if (c == 'x' || c == 'X')
//        {
//            if (base == 0 || base == 16)
//            {
//                ok = 0;
//                base = 16;
//                c = *nptr++;
//            }
//        }
//        else if (base == 0)
//            base = 8;
//    }
//
//    if (base == 0) base = 10;
//
//    {
//        unsigned long dhigh = 0, dlow = 0;
//        int digit;
//
//        while ((digit = _chval(c, base)) >= 0)
//        {
//            ok = 1;
//            dlow = base * dlow + digit;
//            dhigh = base * dhigh + (dlow >> 16);
//            dlow &= 0xffff;
//            c = *nptr++;
//        }
//        if (endptr)
//            *endptr = ok ? (char *) nptr - 1 : (char *) nsptr;
//        /* extra result */
//#ifdef ERRNO
//        return overflowed ? (errno = ERANGE, ULONG_MAX)
//        : (dhigh << 16) | dlow;
//#else
//        return (dhigh << 16) | dlow;
//#endif
//    }
//}
//
//
///*
// *  The way negation is treated in this may not be quite right ...
// */
//uint32_t
//hw_strtoul(const char *nsptr,
//           char **endptr,
//           int base)
//{
//    const unsigned char *nptr = (const unsigned char *) nsptr;
//    int flag = 0, c;
//
//    if (0 == nsptr)
//    {
//        return 0;
//    }
//    else if (0 == *nsptr)
//    {
//        return 0;
//    }
//
//#ifdef ERRNO
//    int errno_saved = errno;
//#endif
//    while (((c = *nptr++) != 0) && hw_isspace(c));
//    nptr--;
//
//#ifdef ERRNO
//    errno = 0;
//#endif
//
//    {
//        char *endp;
//        unsigned long int ud = _strtoul((char *) nptr, &endp, base);
//
//        if (endptr)
//        {
//            *endptr = endp == (char *) nptr ? (char *) nsptr : endp;
//        }
//
//        /*
//         *  The following lines depend on the facts that
//         *  unsigned->int casts and unary '-' cannot cause arithmetic traps.
//         *  Recode to avoid this?
//         */
//#ifdef ERRNO
//        if (errno == ERANGE)
//        return (uint32_t) ud;
//    errno = errno_saved;
//#endif
//        return (uint32_t) ud;
//    }
//}
//
//
///*
// * The specification in the ANSI information bulletin upsets me here:
// * strtol is of type long int, and 'if the correct value would cause
// * overflow LONG_MAX or LONG_MIN is returned'. Thus for hex input the
// * string 0x80000000 will be considered to have overflowed, and so will
// * be returned as LONG_MAX.
// * These days one should use strtoul for unsigned values, so some of
// * my worries go away.
// */
//int32_t
//hw_strtol(const char *nsptr,
//          char **endptr,
//          int base)
//{
//    const unsigned char *nptr = (const unsigned char *) nsptr;
//    int flag = 0;
//    int c;
//
//    if (0 == nsptr)
//    {
//        return 0;
//    }
//    else if (0 == *nsptr)
//    {
//        return 0;
//    }
//
//    while (((c = *nptr++) != 0) && hw_isspace(c));
//
//    switch (c)
//    {
//        case '-':
//            flag |= NUMNEG;
//            /* drop through */
//        case '+':
//            break;
//        default:
//            nptr--;
//            break;
//    }
//
//    {
//        char *endp;
//        unsigned long ud = _strtoul((char *) nptr, &endp, base);
//
//        if (endptr)
//        {
//            *endptr = endp == (char *) nptr ? (char *) nsptr : endp;
//        }
//
//        /*
//         *  The following lines depend on the facts that
//         *  unsigned->int casts and unary '-' cannot cause arithmetic traps.
//         *  Recode to avoid this?
//         */
//#ifdef ERRNO
//        if (flag & NUMNEG) {
//        return (-(long) ud <= 0) ? -(long) ud : (errno =
//                             ERANGE, LONG_MIN);
//        }
//    else {
//        return (+(long) ud >= 0) ? +(long) ud : (errno =
//                             ERANGE, LONG_MAX);
//        }
//#else
//        if (flag & NUMNEG)
//        {
//            ud = -ud;
//        }
//
//        return (int32_t) ud;
//#endif
//    }
//
//}
//
///*  hw_strtol()  */
//
//
//
//int32_t
//hw_atoi(const char *nsptr)
//{
//    return (int32_t) hw_strtol(nsptr, 0, 0);
//}
//
//
//int32_t
//hw_atol(const char *nsptr)
//{
//    return (int32_t) hw_strtol(nsptr, 0, 0);
//}
//
