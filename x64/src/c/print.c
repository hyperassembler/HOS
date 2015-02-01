#include "kdef.h"
#include "type.h"
#include "mem.h"
#include "print.h"

uint64_t text_pos;

uint64_t HYPKERNEL64 hk_str_len(char const * str)
{
    uint32_t length = 0;
    if(str == NULL)
        return 0;
    while(*str != 0)
    {
        str++;
        length++;
    }
    return length;
}

uint64_t HYPKERNEL64 hk_str_cmp(char const * str1,char const * str2)
{
    if(str1 == NULL || str2 == NULL)
        return 0;
    uint32_t length = hk_str_len(str1);
    if(length != hk_str_len(str2))
        return 0;
    while(length--)
    {
        if(*(str1+length) != *(str2+length))
            return 0;
    }
    return 1;
}

void HYPKERNEL64 hk_print_scroll()
{
    hk_mem_move((void*)(0xb8000 + get_pos(1,0) * 2), (void*)(0xb8000 + get_pos(0,0) * 2), (80*24)*2);
    return;
}

void HYPKERNEL64 hk_print_str(char const *str)
{
    if(str == NULL)
        return;
    while (*str != 0)
    {
        if(*str == '\n')
        {
            text_pos = 80 * (get_row(text_pos) + 1);
            if(text_pos > 80 * 25 - 1)
            {
                //can't hold
                hk_print_scroll();
                hk_mem_set((void*)(0xb8000 + 80*24*2), 0, 80 * 2); // clear last row
                text_pos = 80 * 24;
            }
            str++;
        }
        else
        {
            if (text_pos > 80 * 25 - 1)
            {
                //can't hold
                hk_print_scroll();
                text_pos = 80 * 24;
            }
            *((char*)(0xb8000) + text_pos*2) = *str;
            *((char*)(0xb8000) + text_pos*2 + 1) = 7;
            str++;
            text_pos++;
        }
    }
    return;
}

void HYPKERNEL64 hk_print_int(int64_t number)
{
    char arr[21]; // do not need to initialize
    arr[20] = 0; //zero-terminated
    uint32_t index = 19;
    uint32_t isNegative = 0;
    uint32_t const div = 10;
    if (number < 0)
    {
        isNegative = 1;
        number *= -1;
    }
    while (1)
    {
        int64_t quo = number / div;
        int64_t rmd = number % div;
        number = quo;
        arr[index] = (char) ('0' + rmd);
        index--;
        if (number == 0)
            break;
    }
    if (isNegative)
    {
        arr[index] = '-';
        hk_print_str(&(arr[index]));
    }
    else
        hk_print_str(&(arr[index+1]));
    return;
}

void HYPKERNEL64 hk_print_hex(uint64_t number)
{
    const char lookup_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char arr[19];
    arr[18] = 0; //zero-terminated
    uint32_t index = 17;
    uint32_t const div = 16;
    while (1)
    {
        uint64_t quo = number / div;
        uint64_t rmd = number % div;
        number = quo;
        arr[index--] = lookup_table[rmd];
        if (number == 0)
            break;
    }
    arr[index--] = 'x';
    arr[index] = '0';
    hk_print_str(&(arr[index]));
    return;
}

void HYPKERNEL64 hk_clear_screen(void)
{
    text_pos = 0; // reset text_pos
    hk_mem_set((void*)0xb8000, 0, 25*80*2);
    return;
}
