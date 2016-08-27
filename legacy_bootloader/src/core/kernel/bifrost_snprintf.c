/*-----------------------------------------------------
 |
 |      hw_vsnprintf.c
 |
 |  Server-side implementation of printf for bifrost
 |  Only takes a single string parameter.
 |
 |------------------------------------------------------
 |
 |  Revision History :
 |
 |    #5 -  31 January 2005     J. Hanes
 |          Add cts_print_level, cts_print_control()
 |
 |    #4 -  18 October 2004     J. Hanes
 |          Add cts_print_string and cts_print_hex
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2003  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */


#include <bifrost_types.h>
#include <bifrost_intr.h>
#include <conf_defs.h>
#include <math.h>

#define FLG_LONG      (1 << 1)
#define FLG_DLONG     (1 << 2)
#define FLG_ZERO      (1 << 3)
#define FLG_PLUS      (1 << 4)
#define FLG_MINUS     (1 << 5)
#define FLG_SPACE     (1 << 6)
#define FLG_HASH      (1 << 7)

#define GetCh()    (*fmt++)
#define PutCh(ch)  do                                      \
                   {                                       \
                       if ( numChars < bufsize - 1 )       \
                       {                                   \
                           outstring[ numChars++ ] = (ch); \
                       }                                   \
                       else                                \
                       {                                   \
                           if(ch){};                       \
                       }                                   \
                   } while(FALSE)                          \


static uint32_t
local_strlen( const char * s )
{
    int len = 0;
    while ( s[ len ] )
    {
        len++;
    }
    
    return len;
}

int
hw_vsnprintf( char* outstring,
              uint32_t bufsize,
              const char* fmt, 
              va_list args      )
{
    const char* prefix;
    const char* value;
    char       postfix [  8 ];
    char       buf     [ 64 ];
    char       ch;
    uint32_t   numChars;
    int32_t    flags;
    int32_t    width;
    int32_t    precision;
    int32_t    len;
    int32_t    leading_zeroes;
    int32_t    ii;
    int      done;
    
    numChars = 0;
    
    while ( numChars < bufsize - 1 )
    {
        ch = GetCh();
        if (ch == 0)
        {
            break;
        }
        
        if ( ch != '%' )
        {
            PutCh(ch);
            continue;
        }
        
        /*  %[flags][width][.precision][size]type  */
        
        /* isolate the flags */
        flags = 0;
        done = 0;
        do
        {
            ch = GetCh();
            switch ( ch )
            {
            case '-':
                flags |= FLG_MINUS;
                break;
            case '+':
                flags |= FLG_PLUS ;
                break;
            case ' ':
                flags |= FLG_SPACE;
                break;
            case '0':
                flags |= FLG_ZERO ;
                break;
            default:
                done = 1;
                break;
            }
        } while ( !done );
        
        /* get the field width */
        if ( ch == '*' )
        {
            width = va_arg( args, int );
            if (width < 0)
            {
                flags |= FLG_MINUS;
                width = -width;
            }
            
            ch = GetCh();
        }
        else if ((ch >= '0') && (ch <= '9'))
        {
            width = 0;
            do
            {
                width = width * 10 + (ch - '0');
                ch = GetCh();
            } while ((ch >= '0') && (ch <= '9'));
        }
        else
        {
            /* unlimited */
            width = -1;
        }
        
        /*  get the field precision  */
        if ( ch == '.' )
        {
            ch = GetCh();
            if ( ch == '*' )
            {
                precision = va_arg( args, int );
                
                if (precision < 0)
                {
                    precision = 0;
                }
                
                ch = GetCh();
            }
            else
            {
                precision = 0;
                while ((ch >= '0') && (ch <= '9'))
                {
                    precision = precision * 10 + (ch - '0');
                    ch = GetCh();
                }
            }
        }
        else
        {
            /* unlimited */
            precision = -1;
        }
        
        /* isolate the size */
        while ( 1 )
        {
            if ( ch == 'l' )
            {
                ch = GetCh();
                if (ch == 'l')
                {
                    flags |= FLG_DLONG;
                    ch = GetCh();
                }
                else
                {
                    flags |= FLG_LONG;
                }
            }
            else if (ch == 'L')
            {
                flags |= FLG_DLONG;
                ch = GetCh();
            }
            else
            {
                break;
            }
        }
        
        value  = "";
        len    = 0;
        prefix = "";
        postfix[0]      = 0;
        leading_zeroes  = 0;
        
        switch ( ch )
        {
        case 'd':
        case 'i':
        {
            int64_t num;
            
            if ( flags & FLG_DLONG )
            {
                num = va_arg( args, int64_t );
            }
            else
            {
                num = va_arg( args, int );
            }
            
            if (num < 0)
            {
                num = -num;
                prefix = "-";
            }
            
            if (precision < 0)
            {
                precision = 1;
            }
            
            len = 0;
            do
            {
                len ++;
                buf[ sizeof(buf) - len ] = ((char) (num % 10)) + '0';
                num /= 10;
            } while ( num );
            value = &buf[ sizeof(buf) - len ];
            
            if ( precision > len )
            {
                leading_zeroes = precision - len;
                len = precision;
            }
            
            break;
        }
        case 'u':
        {
            uint64_t num;
            
            if (flags & FLG_DLONG)
            {
                num = va_arg( args, uint64_t );
            }
            else
            {
                num = va_arg( args, unsigned int );
            }
            
            if (precision < 0)
            {
                precision = 1;
            }
            
            len = 0;
            do
            {
                len++;
                buf[sizeof(buf) - len] = ((char) (num % 10)) + '0';
                num /= 10;
            } while (num);
            value = &buf[ sizeof(buf) - len ];
            
            if ( precision > len )
            {
                leading_zeroes = precision - len;
                len = precision;
            }
            break;
        }
        case 'e':   /* [-]d.dddddde[+/-]dd                             */
        case 'E':   /* [-]d.ddddddE[+/-]dd                             */
        case 'f':   /* [-]d.dddddd                                     */
        case 'g':   /* if exp < -4 or exp > prec use 'e', else use 'f' */
        case 'G':   /* if exp < -4 or exp > prec use 'E', else use 'f' */
        {
            uint64_t temp, temp2;
            int32_t count;
            bool negative = FALSE;
            double num, fraction;
            num = va_arg( args, double );
            if (precision < 0)
            {
                precision = 1;
            }
            
            // Special case: value is NaN
            if(isnan(num))
            {
                value = "nan";
                len = 3;
                break;
            }
            
            if(num < 0)
            {
                num = -num;
                negative = TRUE;
            }
            
            //
            // Assert that the value to be printed does not
            // exceed the limit of a uint64_t.
            //
            //hw_assert(num < (double) ~0ull);
            
            temp = (uint64_t) num;
            fraction = num - temp;
            
            len = 0;
            count = 0;
            do
            {
                len++;
                count++;
                fraction *= 10;
                temp2 = (uint64_t) fraction;
                buf[sizeof(buf) - precision + len - 1] = ((char) (temp2 % 10)) + '0';
            } while (count < precision);
            len++;
            buf[sizeof(buf) - len] = '.';
            do
            {
                len++;
                buf[sizeof(buf) - len] = ((char) (temp % 10)) + '0';
                temp /= 10;
            } while (temp);
            
            if(negative)
            {
                len++;
                buf[sizeof(buf) - len] = '-';
            }
            
            value = &buf[ sizeof(buf) - len ];
            break;
        }
        case 'x':
        case 'X':
        {
            uint64_t num;
            const char *map;
            
            if (flags & FLG_DLONG)
            {
                num = va_arg( args, uint64_t );
            }
            else
            {
                num = va_arg( args, unsigned int);
            }
            
            if( ch == 'x')
            {
                map = "0123456789abcdef";
            }
            else
            {
                map = "0123456789ABCDEF";
            }
            
            if ( precision < 0 )
            {
                precision = 1;
            }
            
            len = 0;
            do
            {
                len ++;
                buf[ sizeof(buf) - len ] = map[ num & 15 ];
                num /= 16;
            } while ( num );
            value = &buf[ sizeof(buf) - len ];
            
            if (precision > len)
            {
                leading_zeroes = precision - len;
                len = precision;
            }
            break;
        }
        case 's':
            value = va_arg( args, char * );
            if ( 0 == value )
            {
                value = "<NULL>";
            }
            while ( (precision < 0 || len < precision) && value[len] )
            {
                len++;
            }
            break;
        case 0:
            PutCh('%');
            goto exit;
            break;
        case 'c':
            buf[0]   = (char) (va_arg( args, int ) & 0xFF);
            value    = buf;
            len      = 1;
            break;
            
        default:
            /*
             *  Handle unknown formatting commands
             *  by just printing the command character using the
             *  specified width and precision. This is how %% is
             *  handled.
             */
            buf[ 0 ] = ch;
            value    = buf;
            len      = 1;
            break;
        }
        
        if (prefix[0] == 0)
        {
            if (flags & FLG_PLUS)
            {
                prefix = "+";
            }
            else if (flags & FLG_SPACE)
            {
                prefix = " ";
            }
        }
        else if (flags & FLG_ZERO)
        {
            /* prefix goes before zero padding, but after space padding */
            ii = 0;
            while ( prefix[ ii ] )
            {
                PutCh( prefix[ ii ++ ] );
            }
            width -= ii;
        }
        
        if ((width > 0) && ((flags & FLG_MINUS) == 0))
        {
            int jj;
            char fillChar;
            
            jj = width - local_strlen( prefix ) - len - local_strlen( postfix );
            fillChar = (flags & FLG_ZERO) ? '0' : ' ';
            if ( jj > 0 )
            {
                width -= jj;
                while ( jj -- > 0 )
                {
                    PutCh( fillChar );
                }
            }
        }
        
        if ( (flags & FLG_ZERO) == 0 )
        {
            ii = 0;
            while ( prefix[ ii ] )
            {
                PutCh( prefix[ ii ++ ] );
            }
            width -= ii;
        }
        
        len -= leading_zeroes;
        while ( leading_zeroes -- )
        {
            PutCh( '0' );
        }
        
        ii = 0;
        while ( ii < len )
        {
            PutCh( value[ ii ++ ] );
        }
        
        width -= len;
        
        ii = 0;
        while ( postfix[ ii ] )
        {
            PutCh( postfix[ ii ++ ] );
        }
        width -= ii;
        
        if (flags & FLG_MINUS)
        {
            /* handle padding for left-justified field */
            if (width > 0)
            {
                while (width-- > 0)
                {
                    PutCh(' ');
                }
            }
        }
    }
    
exit:
    outstring[ numChars ] = 0;
    return numChars;
}

int hw_snprintf( char * outstring, uint32_t size, const char * fmt, ... )
{
    int ret;
    
    va_list   args;
    va_start( args, fmt );
    ret = hw_vsnprintf( outstring, size, fmt, args );
    va_end( args );
    
    return ret;
}

#define PRINTBUF_SIZE (1024)
extern int plat_puts( const char* buffer, int str_length );
char printbuf[HW_PROC_CNT][PRINTBUF_SIZE];

void hw_printf(const char *format, ...)
{
    hw_irql_t prev_irql = ke_raise_irql(HW_IRQL_DISABLED_LEVEL);
    int corenum = ke_get_current_core();
    int len = hw_snprintf(printbuf[corenum], PRINTBUF_SIZE, "Core %d: ", ke_get_current_core());
    va_list argptr;
    va_start(argptr, format);
    len += hw_vsnprintf(&printbuf[corenum][len], PRINTBUF_SIZE - len, format, argptr);
    va_end(argptr);
    
    plat_puts(printbuf[corenum], len);
    ke_lower_irql(prev_irql);
}

