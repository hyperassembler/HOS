/*-----------------------------------------------------
 |
 |      hw_strtof.c
 |
 |------------------------------------------------------
 |
 |  Revision History :
 |
 |    #2 -  20 January 2005 J. Hanes
 |          pick lint
 |
 |    #1 -  2 March 2004    J. Hanes
 |          Ooops, forgot to p5 add it.
 |       -  8 February 2004 J. Hanes
 |          Copied/modified from
 |          hwdev/antichips/antinemo/sysver/lib/src/shared/SSW_stdlib/strtof.c
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2004  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"
#include <ctype.h>

#define MAX10 (6)

static float tentothe2tothe[ MAX10 ] =
    { 1.0e1, 1.0e2, 1.0e4, 1.0e8, 1.0e16, 1.0e32 };

static float
hw_pow10(int exp)
{
    int i = MAX10, j = 32, minus;
    float f = 1.0;

    if ( (minus = (exp < 0)) )
    exp = -exp;

    while (--i >= 0) {
    if (exp >= j) {
        f *= tentothe2tothe[i];
        exp -= j;
    }
    j >>= 1;
    }

    return (minus) ? (1.0 / f) : f;
}



float
hw_strtof( const char *  str, 
                 char ** endScan )
{
    float x = 0.0, div;
    int negsign, exp = 0, expsign;

    /* eat any leading whitespace */
    while ( (hw_isspace( (int) *str )) ) {
    str++;
    }

    if ( (negsign = (*str == '-')) ) {
    str++;
    }
    else if ( *str == '+' ) {
    str++;
    }

    /* now read in the first part of the number */
    while (isdigit((int)*str))
    x = 10.0 * x + (*str++ - '0');

    /* if we hit a period, do the decimal part now */
    if (*str == '.') {
    str++;
    div = 10.0;
    while (isdigit((int)*str)) {
        x += (*str++ - '0') / div;
        div *= 10.0;
    }
    }

    /* check for an exponent */
    if ((*str == 'e') || (*str == 'E')) {
    str++;

    if ( (expsign = (*str == '-')) )
        str++;
    else if (*str == '+')
        str++;

    /* handle leading zeros, such as in 1.0e-07 or 1.0e001 */
    while (*str == '0')
        str++;

    /* now do the exponent */
    while (isdigit((int)*str))
        exp = 10 * exp + (*str++ - '0');
    if (expsign)
        exp = -exp;

    if (exp)
        x *= hw_pow10(exp);
    }

    if (negsign)
    x = -x;

    if (endScan)
    *endScan = (char *) str;

    return x;
}
