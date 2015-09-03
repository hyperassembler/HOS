#include <stdarg.h>
#include "kdef.h"
#include "type.h"
#include "mem.h"
#include "print.h"

uint32_t text_pos;

uint32_t NATIVE32 str_len(char const *str)
{
    uint32_t length = 0;
    if (str == NULL)
        return 0;
    while (*str != 0)
    {
        str++;
        length++;
    }
    return length;
}

uint32_t NATIVE32 str_cmp(char const *str1, char const *str2)
{
    if (str1 == NULL || str2 == NULL)
        return 0;
    uint32_t length = str_len(str1);
    if (length != str_len(str2))
        return 0;
    while (length--)
    {
        if (*(str1 + length) != *(str2 + length))
            return 0;
    }
    return 1;
}

void NATIVE32 _print_scroll()
{
    mem_move((void *) (0xb8000 + get_pos(1, 0) * 2), (void *) (0xb8000 + get_pos(0, 0) * 2), (80 * 24) * 2);
    return;
}

void NATIVE32 _print_str(char const *str)
{
    if (str == NULL)
        return;
    while (*str != 0)
    {
        if (*str == '\n')
        {
            text_pos = 80 * (get_row(text_pos) + 1);
            if (text_pos > 80 * 25 - 1)
            {
                //can't hold
                _print_scroll();
                mem_set((void *) (0xb8000 + 80 * 24 * 2), 0, 80 * 2); // clear last row
                text_pos = 80 * 24;
            }
            str++;
        }
        else
        {
            if (text_pos > 80 * 25 - 1)
            {
                //can't hold
                _print_scroll();
                text_pos = 80 * 24;
            }
            *((char *) (0xb8000) + text_pos * 2) = *str;
            *((char *) (0xb8000) + text_pos * 2 + 1) = 7;
            str++;
            text_pos++;
        }
    }
    return;
}

void NATIVE32 _print_uint(uint32_t number)
{
    char arr[11]; // do not need to initialize
    arr[10] = 0; //zero-terminated
    uint32_t index = 9;
    uint32_t const div = 10;
    while (1)
    {
        uint32_t quo = number / div;
        uint32_t rmd = number % div;
        number = quo;
        arr[index] = (char) ('0' + rmd);
        index--;
        if (number == 0)
            break;
    }
    _print_str(&(arr[index + 1]));
    return;
}


void NATIVE32 _print_int(int32_t number)
{
    char arr[12]; // do not need to initialize
    arr[11] = 0; //zero-terminated
    uint32_t index = 10;
    uint32_t isNegative = 0;
    uint32_t const div = 10;
    if (number < 0)
    {
        isNegative = 1;
        number *= -1;
    }
    while (1)
    {
        int32_t quo = number / div;
        int32_t rmd = number % div;
        number = quo;
        arr[index] = (char) ('0' + rmd);
        index--;
        if (number == 0)
            break;
    }
    if (isNegative)
    {
        arr[index] = '-';
        _print_str(&(arr[index]));
    }
    else
        _print_str(&(arr[index + 1]));
    return;
}

void NATIVE32 _print_hex(uint32_t number, uint32_t captial)
{
    char const lookup_table_cap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char const lookup_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char const * const look_up = captial == 1 ? &lookup_table_cap[0] : &lookup_table[0];
    char arr[9];
    arr[8] = 0; //zero-terminated
    uint32_t index = 7;
    uint32_t const div = 16;
    while (1)
    {
        uint32_t quo = number / div;
        uint32_t rmd = number % div;
        number = quo;
        arr[index--] = look_up[rmd];
        if (number == 0)
            break;
    }
    _print_str(&(arr[index + 1]));
    return;
}


void NATIVE32 clear_screen(void)
{
    text_pos = 0; // reset text_pos
    mem_set((void *) 0xb8000, 0, 25 * 80 * 2);
    return;
}


void NATIVE32 kprintf(char const *format, ...)
{
    va_list args;
    va_start(args, format);
    char buf[2] = {0, 0};
    int32_t d;
    uint32_t u;
    char* s;
    char c;
    for(;*format != '\0';format++)
    {
        if (*format != '%')
        {
            buf[0] = *format;
            _print_str(buf);
            continue;
        }
        format++;
        switch (*format)
        {
            case 'd':
                d = va_arg(args, int32_t);
                _print_int(d);
                break;
            case 'u':
                u = va_arg(args, uint32_t);
                _print_uint(u);
                break;
            case 's':
                s = va_arg(args, char *);
                _print_str(s);
                break;
            case 'c':
                c = va_arg(args, int32_t);
                buf[0] = c;
                _print_str(buf);
                break;
            case 'x':
                u = va_arg(args, uint32_t);
                _print_hex(u, 0);
                break;
            case 'X':
                u = va_arg(args, uint32_t);
                _print_hex(u, 1);
                break;
            case '%':
                buf[0] = '%';
                _print_str(buf);
                break;
            default:
                buf[0] = '%';
                _print_str(buf);
                format--;
                break;
        }
    }
}