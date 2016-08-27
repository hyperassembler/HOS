/*-----------------------------------------------------
 |
 |      hw_strings.c
 |
 |  Safe implementations of
 |    strlen()
 |    strncpy()
 |    strncat()
 |
 |  Not-very-safe implementation of
 |    strcmp()
 |
 |------------------------------------------------------
 |
 |  Revision History : 
 |
 |    #5 - 15 February 2005 J. Hanes
 |         Drop hw_api_trace_enable; use SHARED_ERRMSG, SHARED_WARNING, SHARED_TRACE
 |
 |    #4 -  11 August 2004  J. Hanes
 |          Pick lint.
 |
 |    #3 -  31 March 2004   J. Hanes
 |          Add hw_strcmp()
 |
 |    #2 -  12 January 2004 J. Hanes
 |          Use ctp_printf() instead of HW_printf()
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2003  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"

#include <string.h>

SIZE_T
hw_strnlen( const char* s, SIZE_T maxlen ) 
{
    static const char* const myname = "hw_strnlen";
    SIZE_T len = 0;

    if ( 0 == s ) {
        hw_errmsg( "%s: Error: NULL parameter s\n", myname );
    }
    while ( len < maxlen && s[ len ] ) {
        len ++ ;
    }
    if ( len >= maxlen ) {
        hw_errmsg( "%s: Error: reached buffer length limit %d\n"
                    "\twithout finding a terminating NULL\n", myname, maxlen );
    }
    return len;
}


char*
hw_strncpy( char* s1, const char* s2, SIZE_T n )
{
    SIZE_T ii = 0;

    if ( 0 == s1 ) {
        hw_errmsg( "%s: Error: NULL destination parameter s1\n", __func__ );
    }
    else if ( 0 == s2 )  {
        hw_errmsg( "%s: Error: NULL source parameter s2\n", __func__ );
    }
    else {
        for ( ii = 0; ii < n; ii ++ ) {
            s1[ ii ] = s2[ ii ];
            if('\0' == s2[ii])
            {
                break;
            }
        }
    }
    if(ii >= n && n > 0) // if length was non-zero and we reached the end without finding null character
    {
        s1[n - 1] = '\0';
    }
    return( s1 );
}

char*
hw_strncpytomem( char* s1, const char* s2, SIZE_T n )
{
    SIZE_T ii = 0;

    if ( 0 == s1 ) {
        hw_errmsg( "%s: Error: NULL destination parameter s1\n", __func__ );
    }
    else if ( 0 == s2 )  {
        hw_errmsg( "%s: Error: NULL source parameter s2\n", __func__ );
    }
    else {
        for ( ii = 0; ii < n; ii ++ ) {
                hw_write8((UINT8*)&s1[ ii ], (UINT8)s2[ ii ]);
                if('\0' == s2[ii])
                {
                    break;
                }
            }
    }
    if(ii >= n && n > 0) // if length was non-zero and we reached the end without finding null character
    {
        hw_write8((UINT8*)&s1[ n - 1 ], (UINT8)'\0');
    }
    return( s1 );
}

char*
hw_strncpyfrommem( char* s1, const char* s2, SIZE_T n )
{
    SIZE_T ii = 0;
    UCHAR byteRead;

    if ( 0 == s1 ) {
        hw_errmsg( "%s: Error: NULL destination parameter s1\n", __func__ );
    }
    else if ( 0 == s2 )  {
        hw_errmsg( "%s: Error: NULL source parameter s2\n", __func__ );
    }
    else {
        for ( ii = 0; ii < n; ii ++ ) {
            byteRead = hw_read8((UINT8*)&s2[ ii ]);
            s1[ ii ] = byteRead;
            if('\0' == byteRead)
            {
                break;
            }
        }
    }
    if(ii >= n && n > 0) // if length was non-zero and we reached the end without finding null character
    {
        s1[n - 1] = '\0';
    }
    return( s1 );
}


char*
hw_strncat( char* s1, const char* s2, SIZE_T n )
{
    unsigned int foundnull = 0;
    SIZE_T sdex;
    SIZE_T ddex;

    if ( 0 == s1 ) { 
        hw_errmsg( "%s: Error: NULL destination parameter s1\n", __func__ );
    }
    else if ( 0 == s2 )  { 
        hw_errmsg( "%s: Error: NULL source parameter s2\n", __func__ );
    }
    else {
        for ( sdex = 0, ddex = 0; sdex < n; ddex ++ ) {
            if ( 0 == s1[ ddex ] ) {
                foundnull = 1;
            }
            if ( 1 == foundnull ) {
                s1[ ddex ] = s2[ sdex ++ ];
            }
        }
    }
    if ( 0 == foundnull ) {
        /*
         *  ALWAYS terminate the string,
         *  truncating the output if necessary.
         */
        s1[ n - 1 ] = 0;
        hw_errmsg( "%s: Error: reached buffer length limit %d\n"
                    "\tOutput string truncated\n", __func__, n );
    }
    return( s1 );

}



INT32
hw_strcmp( const char* str1, const char* str2 )
{
    const char* p1 = str1;
    const char* p2 = str2;

    if ( (0 == str1) || (0 == str2) ) {
        int rc;
        hw_errmsg( "hw_strcmp: Error: got 0 for input string pointer:  "
                     "hw_strcmp( 0x%08x, 0x%08x )\n", str1, str2 );
        if      ( str1 > str2 ) rc =  1;
        else if ( str1 < str2 ) rc = -1;
        else                    rc =  0;
        return( rc );
    }
    else {
        while ( *p1 && *p2 ) {
            if      ( *p1 > *p2 ) return(  1 );
            else if ( *p1 < *p2 ) return( -1 );
            p1++;
            p2++;
        }
        if      ( *p1 > *p2 ) return(  1 );
        else if ( *p1 < *p2 ) return( -1 );
        return(  0 );
    }

}

UINT64 hw_strhash(const CHAR* str)
{
    UINT64 hashval = 5381;
    UINT32 c = 0;
    
    while((c = *str++))
    {
        hashval = ((hashval << 5) + hashval) + c; /* hashval * 33 + c */
    }
    
    return hashval;
}

BOOL hw_isspace(UCHAR ch)
{
    BOOL ret = FALSE;
    switch(ch)
    {
    case ' ':
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
        ret = TRUE;
        break;
    default:
        break;
    }
    
    return ret;
}

BOOL hw_isupper(UCHAR ch)
{
    return (ch >= 'A' && ch <= 'Z');
}

BOOL hw_islower(UCHAR ch)
{
    return (ch >= 'a' && ch <= 'z');
}

BOOL hw_isdigit(UCHAR ch)
{
    return (ch >= '0' && ch <= '9');
}

