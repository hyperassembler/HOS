/*-----------------------------------------------------
 |
 |      hw_strtol.c
 |
 |  hw_strtoul(), hw_strtol(), and hw_atoi()
 |
 |------------------------------------------------------
 |
 |  Revision History :
 |
 |    #3 - 15 February 2005 J. Hanes
 |         Drop hw_api_trace_enable; use SHARED_ERRMSG, SHARED_TRACE
 |
 |    #2 -  28 April 2004   J. Hanes
 |          Check for 0 string pointer in strtol() and strtoul()
 |
 |    #1 -  8 February 2004 J. Hanes
 |          Copied in and modified from
 |          hwdev/antichips/antinemo/sysver/lib/src/shared/SSW_stdlib/strtol.c
 |          Appended atoi() and atol to the bottom.
 |          Replaced NULL with 0
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 1997  CagEnt Technologies Inc.
 |    Copyright (C) 1999  WebTV Networks Inc.
 |    Copyright (C) 2004  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */



#include "bifrost_private.h"
#include <ctype.h>

#define NUMNEG (01000)



static int
_chval(int ch, int radix)
{
    int val;

    val = (hw_isdigit(ch) ? (ch) - '0' :
       hw_islower(ch) ? (ch) - 'a' + 10 :
       hw_isupper(ch) ? (ch) - 'A' + 10 : -1);

    return (val < radix ? val : -1);
}


static unsigned long int
_strtoul( const char *  nsptr, 
                char ** endptr, 
                int     base    )
{
    const unsigned char *nptr = (const unsigned char *) nsptr; /* see scanf */
    int c, ok = 0;

    while (((c = *nptr++) != 0) && hw_isspace(c));
    if (c == '0') {
    ok = 1;
    c = *nptr++;
    if (c == 'x' || c == 'X') {
        if (base == 0 || base == 16) {
        ok = 0;
        base = 16;
        c = *nptr++;
        }
    }
    else if (base == 0)
        base = 8;
    }

    if (base == 0) base = 10;

    {
    unsigned long dhigh = 0, dlow = 0;
    int digit;

    while ((digit = _chval(c, base)) >= 0) {
        ok = 1;
        dlow = base * dlow + digit;
        dhigh = base * dhigh + (dlow >> 16);
        dlow &= 0xffff;
        c = *nptr++;
    }
    if (endptr)
        *endptr = ok ? (char *) nptr - 1 : (char *) nsptr;
    /* extra result */
#ifdef ERRNO
    return overflowed ? (errno = ERANGE, ULONG_MAX)
        : (dhigh << 16) | dlow;
#else
    return (dhigh << 16) | dlow;
#endif
    }
}



/*
 *  The way negation is treated in this may not be quite right ...
 */
UINT32
hw_strtoul( const char *  nsptr, 
                  char ** endptr, 
                  int     base    )
{
    const unsigned char *       nptr   = (const unsigned char *) nsptr;
    int flag = 0, c;

    if ( 0 == nsptr ) {
        return 0;
    }
    else if ( 0 == * nsptr ) {
        return 0;
    }

#ifdef ERRNO
    int errno_saved = errno;
#endif
    while (((c = *nptr++) != 0) && hw_isspace(c));
    nptr--;

#ifdef ERRNO
    errno = 0;
#endif

    {
    char *endp;
    unsigned long int ud = _strtoul((char *) nptr, &endp, base);

    if (endptr) {
        *endptr = endp == (char *) nptr ? (char *) nsptr : endp;
        }

        /*
         *  The following lines depend on the facts that 
         *  unsigned->int casts and unary '-' cannot cause arithmetic traps.  
         *  Recode to avoid this?
         */
#ifdef ERRNO
    if (errno == ERANGE)
        return (UINT32) ud;
    errno = errno_saved;
#endif
    return (UINT32) ud;
    }
}



/*
 * The specification in the ANSI information bulletin upsets me here:
 * strtol is of type long int, and 'if the correct value would cause
 * overflow LONG_MAX or LONG_MIN is returned'. Thus for hex input the
 * string 0x80000000 will be considered to have overflowed, and so will
 * be returned as LONG_MAX.
 * These days one should use strtoul for unsigned values, so some of
 * my worries go away.
 */
INT32
hw_strtol( const char *  nsptr, 
           char       ** endptr, 
           int           base    )
{
    const unsigned char *nptr = (const unsigned char *) nsptr;
    int flag = 0;
    int c;

    if ( 0 == nsptr ) {
        return 0;
    }
    else if ( 0 == * nsptr ) {
        return 0;
    }

    while (((c = *nptr++) != 0) && hw_isspace(c));

    switch (c) {
    case '-':
    flag |= NUMNEG;
    /* drop through */
    case '+':
    break;
    default:
    nptr--;
    break;
    }

    {
    char *endp;
    unsigned long ud = _strtoul((char *) nptr, &endp, base);

    if (endptr) {
        *endptr = endp == (char *) nptr ? (char *) nsptr : endp;
        }

        /*
         *  The following lines depend on the facts that 
         *  unsigned->int casts and unary '-' cannot cause arithmetic traps.  
         *  Recode to avoid this?
         */
#ifdef ERRNO
    if (flag & NUMNEG) {
        return (-(long) ud <= 0) ? -(long) ud : (errno =
                             ERANGE, LONG_MIN);
        }
    else {
        return (+(long) ud >= 0) ? +(long) ud : (errno =
                             ERANGE, LONG_MAX);
        }
#else
    if (flag & NUMNEG) {
            ud = -ud;
        }

    return (INT32) ud;
#endif
    }

}  /*  hw_strtol()  */



INT32
hw_atoi( const char * nsptr )
{
    return (INT32) hw_strtol( nsptr, 0, 0 );
}


INT32
hw_atol( const char *nsptr )
{
    return (INT32) hw_strtol( nsptr, 0, 0 );
}
