#include "common.h"
#include "cpu.h"
#include "print.h"

// #define get_column(pos) ((pos) % 80)
#define get_row(pos) ((pos) / 80)
#define get_pos(row, col) ((row) * 80 + (col))

static uint64 text_pos;

void
hal_print_init(void)
{
    text_pos = 0;
}

static void
halp_print_scroll(void)
{
    mem_mv((void *) (0xb8000 + get_pos(1, 0) * 2), (void *) (0xb8000 + get_pos(0, 0) * 2), (80 * 24) * 2);
}

static void
halp_print_str(char const *str)
{
    if (str == NULL)
    {
        return;
    }
    while (*str != 0)
    {
        if (*str == '\n')
        {
            text_pos = 80 * (get_row(text_pos) + 1);
            if (text_pos > 80 * 25 - 1)
            {
                //can't hold
                halp_print_scroll();
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
                halp_print_scroll();
                text_pos = 80 * 24;
            }
            *((char *) (0xb8000) + text_pos * 2) = *str;
            *((char *) (0xb8000) + text_pos * 2 + 1) = 7;
            str++;
            text_pos++;
        }
    }
}

static void
halp_print_uint(uint64 number)
{
    char arr[21]; // do not need to initialize
    arr[20] = 0; //zero-terminated
    uint32 index = 19;
    uint32 const div = 10;
    while (1)
    {
        uint64 quo = number / div;
        uint64 rmd = number % div;
        number = quo;
        arr[index--] = (char) ('0' + rmd);
        if (number == 0)
        {
            break;
        }
    }
    halp_print_str(&(arr[index + 1]));
}

static void
halp_print_int(int64 number)
{
    char arr[21]; // do not need to initialize
    arr[20] = 0; //zero-terminated
    uint32 index = 19;
    uint32 isNegative = 0;
    uint32 const div = 10;
    if (number < 0)
    {
        isNegative = 1;
        number *= -1;
    }
    while (1)
    {
        int64 quo = number / div;
        int64 rmd = number % div;
        number = quo;
        arr[index--] = (char) ('0' + rmd);
        if (number == 0)
        {
            break;
        }
    }
    if (isNegative)
    {
        arr[index--] = '-';
    }
    halp_print_str(&(arr[index + 1]));
}

static void
halp_print_hex(uint64 number, uint64 capital)
{
    char const lookup_table_cap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char const lookup_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    char const *const look_up = capital == 1 ? &lookup_table_cap[0] : &lookup_table[0];
    char arr[17];
    arr[16] = 0; //zero-terminated
    uint32 index = 15;
    uint32 const div = 16;
    while (1)
    {
        uint64 quo = number / div;
        uint64 rmd = number % div;
        number = quo;
        arr[index--] = look_up[rmd];
        if (number == 0)
        {
            break;
        }
    }
    halp_print_str(&(arr[index + 1]));
}

void
hal_clear_screen(void)
{
    text_pos = 0; // reset text_pos
    mem_set((void *) 0xb8000, 0, 25 * 80 * 2);
}

static void
halp_vprintf(char const *format, va_list args)
{
    char buf[2] = {0, 0};
    int64
            d;
    uint64
            u;
    char *s;
    char c;
    for (; *format != '\0'; format++)
    {
        if (*format != '%')
        {
            buf[0] = *format;
            halp_print_str(buf);
            continue;
        }
        format++;
        switch (*format)
        {
            case 'd':
                d = va_arg(args, int64);
                halp_print_int(d);
                break;
            case 'u':
                u = va_arg(args, uint64);
                halp_print_uint(u);
                break;
            case 's':
                s = va_arg(args, char *);
                halp_print_str(s);
                break;
            case 'c':
                c = (char) va_arg(args, int64);
                buf[0] = c;
                halp_print_str(buf);
                break;
            case 'x':
                u = va_arg(args, uint64);
                halp_print_hex(u, 0);
                break;
            case 'X':
                u = va_arg(args, uint64);
                halp_print_hex(u, 1);
                break;
            case '%':
                buf[0] = '%';
                halp_print_str(buf);
                break;
            default:
                buf[0] = '%';
                halp_print_str(buf);
                format--;
                break;
        }
    }
}

void
hal_printf(char const *format, ...)
{
    va_list args;
    va_start(args, format);
    halp_vprintf(format, args);
    va_end(args);
}

void
hal_assert(uint32 expression, char *message
)
{
    if (!expression)
    {
        hal_printf("HAL: Assertion failed. Detail: %s", message == NULL ? "NULL" : message);

        hal_halt_cpu();
    }
}
