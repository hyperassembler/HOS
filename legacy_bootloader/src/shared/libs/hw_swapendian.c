/*-----------------------------------------------------
 |
 |      hw_swapendian.c
 |
 |------------------------------------------------------
 |
 |  Revision History :
 |
 |    #4 - 25 May 2004      J. Hanes
 |         Add hw_swap_endian_2_bytes()
 |
 |    #3 - 8 February 2004  J. Hanes
 |         Forgot to #include bifrost_port.h  when I
 |           added the call to ctp_am_big_endian()
 |         Drop the #ifdef control over swapping --
 |           it's a runtime thing the user has to take
 |           care of in their test, not compile-time
 |
 |    #2 - 25 January 2004  J. Hanes
 |         Add hw_am_i_big_endian()
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


BOOL
hw_am_i_big_endian( void ) {
    union {
        long l;
        char c[ sizeof (long) ];
    } u;
    u.l = 1;
    return ( u.c[ sizeof (long) - 1 ] == 1 );
}



UINT32
hw_swap_endian32( UINT32 inword  )
{
    UINT32 outword;
    outword = ((inword & 0xff) << 24) | ((inword & 0xff00) << 8) | ((inword >> 8) & 0xff00) | ((inword >> 24) & 0xff);

    return outword;

}




UINT16
hw_swap_endian16( UINT16 inshort  )
{
    UINT16 outshort;

    outshort   = (((inshort & 0xff) << 8) | ((inshort >> 8) & 0xff));

    return outshort;

}

