#include "../common/sys/kdef.h"
#include "../common/sys/type.h"
#include "../common/lib/mem.h"
#include "print.h"
#include "var.h"

void NATIVE64 _hal_print_scroll()
{
    mem_move((void *) (0xb8000 + get_pos(1, 0) * 2), (void *) (0xb8000 + get_pos(0, 0) * 2), (80 * 24) * 2);
    return;
}

void NATIVE64 _hal_print_str(char const *str)
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
                _hal_print_scroll();
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
                _hal_print_scroll();
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

void NATIVE64 _hal_print_uint(uint64_t number)
{
    char arr[21]; // do not need to initialize
    arr[20] = 0; //zero-terminated
    uint32_t index = 19;
    uint32_t const div = 10;
    while (1)
    {
        uint64_t quo = number / div;
        uint64_t rmd = number % div;
        number = quo;
        arr[index--] = (char) ('0' + rmd);
        if (number == 0)
            break;
    }
    _hal_print_str(&(arr[index + 1]));
    return;
}

void NATIVE64 _hal_print_int(int64_t number)
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
        arr[index--] = (char) ('0' + rmd);
        if (number == 0)
            break;
    }
    if (isNegative)
    {
        arr[index--] = '-';
    }
    _hal_print_str(&(arr[index + 1]));
    return;
}

void NATIVE64 _hal_print_hex(uint64_t number, uint64_t capital)
{
    char const lookup_table_cap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char const lookup_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char const * const look_up = capital == 1 ? &lookup_table_cap[0] : &lookup_table[0];
    char arr[17];
    arr[16] = 0; //zero-terminated
    uint32_t index = 15;
    uint32_t const div = 16;
    while (1)
    {
        uint64_t quo = number / div;
        uint64_t rmd = number % div;
        number = quo;
        arr[index--] = look_up[rmd];
        if (number == 0)
            break;
    }
    _hal_print_str(&(arr[index + 1]));
    return;
}

void NATIVE64 hal_clear_screen(void)
{
    text_pos = 0; // reset text_pos
    mem_set((void *) 0xb8000, 0, 25 * 80 * 2);
    return;
}

void NATIVE64 hal_printf(char const *format, ...)
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
            _hal_print_str(buf);
            continue;
        }
        format++;
        switch (*format)
        {
            case 'd':
                d = va_arg(args, int64_t);
                _hal_print_int(d);
                break;
            case 'u':
                u = va_arg(args, uint64_t);
                _hal_print_uint(u);
                break;
            case 's':
                s = va_arg(args, char *);
                _hal_print_str(s);
                break;
            case 'c':
                c = va_arg(args, int64_t);
                buf[0] = c;
                _hal_print_str(buf);
                break;
            case 'x':
                u = va_arg(args, uint64_t);
                _hal_print_hex(u, 0);
                break;
            case 'X':
                u = va_arg(args, uint64_t);
                _hal_print_hex(u, 1);
                break;
            case '%':
                buf[0] = '%';
                _hal_print_str(buf);
                break;
            default:
                buf[0] = '%';
                _hal_print_str(buf);
                format--;
                break;
        }
    }
}
