#include <kern/print.h>
#include <arch/print.h>
#include <kern/libkern.h>
#include <kern/spin_lock.h>

/* max space needed for each byte is when printing it in binary = 8 bits */
#define NBUF_SZ (sizeof(uintmax) * 8)

static char nbuf[NBUF_SZ];
static struct spin_lock print_lock = SPIN_LOCK_INITIALIZER;

static int
_printu(char *buf, uintmax num, uint base, int cap)
{
    int len;
    char c;

    len = 0;
    while (num > 0) {
        c = dtoa(num % base);
        if (cap) {
            c = (char) toupper(c);
        }
        buf[len] = c;

        len++;
        num = num / base;
    }

    return len;
}

static int
_vprintf(const char *fmt, va_list args)
{
    uintmax num;
    char c;
    const char * s;
    int base, usignf, capf, sz_ptr, sz_long, len, ret;

    ret = 0;

    for (; *fmt != '\0'; fmt++) {
        if (*fmt != '%') {
            arch_putc(*fmt);
            continue;
        }

        base = 10;
        usignf = 0;
        sz_ptr = 0;
        sz_long = 0;
        capf = 0;

        retry:
        fmt++;
        switch (*fmt) {
            case 'p':
                sz_ptr = 1;
                goto pnum;
            case 'd':
                goto pnum;
            case 'u':
                usignf = 1;
                goto pnum;
            case 's':
                s = (char*)va_arg(args, char *);
                while (*s != '\0') {
                    arch_putc(*s);
                    s++;
                    ret++;
                }
                break;
            case 'c':
                c = (char) va_arg(args, int);
                arch_putc(c);
                ret++;
                break;
            case 'x':
                base = 16;
                usignf = 1;
                goto pnum;
            case 'X':
                base = 16;
                usignf = 1;
                capf = 1;
                goto pnum;
            case 'l':
                sz_long = 1;
                goto retry;
            case '%':
                arch_putc('%');
                ret++;
                break;
            default:
                /* ignore */
                break;
            pnum:
                if (usignf) {
                    if (sz_ptr) {
                        num = (uintptr) va_arg(args, uintptr);
                    } else if (sz_long) {
                        num = (ulong) va_arg(args, ulong);
                    } else {
                        num = (uint) va_arg(args, uint);
                    }
                } else {
                    if (sz_ptr) {
                        num = (uintptr) va_arg(args, uintptr);
                    } else if (sz_long) {
                        num = (long) va_arg(args, long);
                    } else {
                        num = (int) va_arg(args, int);
                    }
                }
                /* print num */
                if (!usignf && (intmax) num < 0) {
                    num = -(intmax)num;
                    arch_putc('-');
                }

                len = _printu(nbuf, num, base, capf);
                while (len) {
                    arch_putc(nbuf[len]);
                    len--;
                }
                ret += len;
        }
    }

    return ret;
}

ATTR_FMT_PRINTF int
kprintf(const char *fmt, ...)
{
    int ret;
    va_list args;
    va_start(args, fmt);

    spin_lock_acq(&print_lock);
    ret = _vprintf(fmt, args);
    spin_lock_rel(&print_lock);

    va_end(args);
    return ret;
}

int
kvprintf(const char *fmt, va_list args)
{
    int ret;

    spin_lock_acq(&print_lock);
    ret = _vprintf(fmt, args);
    spin_lock_rel(&print_lock);

    return ret;
}
