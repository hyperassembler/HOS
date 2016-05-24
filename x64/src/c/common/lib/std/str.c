/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "std_lib.h"

uint64_t SAPI str_len(char const *str)
{
    uint64_t length = 0;
    if(str == NULL)
        return 0;
    while(*str != 0)
    {
        str++;
        length++;
    }
    return length;
}

uint64_t SAPI str_cmp(char const *str1, char const *str2)
{
    if(str1 == NULL || str2 == NULL)
        return 0;
    uint64_t length = str_len(str1);
    if(length != str_len(str2))
        return 0;
    while(length--)
    {
        if(*(str1+length) != *(str2+length))
            return 0;
    }
    return 1;
}