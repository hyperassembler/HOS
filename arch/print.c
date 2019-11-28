#include <kern/cdef.h>
#include <arch/mem.h>
#include <kern/libkern.h>

#define FB_PADDR (0xb8000)
#define FB_ROW (25)
#define FB_COL (80)
#define BYTE_PER_CHAR (2)
#define FB_SZ (FB_ROW * FB_COL * BYTE_PER_CHAR)
#define DEFAULT_COLOR (0x07)

static char *base;
static uint text_pos;

static void
_fb_scroll()
{
    memmove(base, base + FB_COL * BYTE_PER_CHAR, FB_SZ - (FB_COL * BYTE_PER_CHAR));
    text_pos = FB_SZ - (FB_COL * BYTE_PER_CHAR);
}

static void
_print_newline(void)
{
    text_pos += FB_COL * BYTE_PER_CHAR - text_pos % (FB_COL * BYTE_PER_CHAR);

    if (text_pos >= FB_SZ) {
        _fb_scroll();
    }
}

void
arch_print_init(void)
{
    // 0 here since it doesn't matter direct mapped
    base = arch_pmap_map(FB_PADDR, FB_SZ);
    text_pos = 0;
}

void
arch_cls()
{
    memset(base, 0, FB_SZ);
}

void
arch_putc(const char c)
{
    if (c == '\n') {
        _print_newline();
        return;
    }

    if (text_pos >= FB_SZ) {
        _fb_scroll();
    }

    base[text_pos++] = c;
    base[text_pos++] = DEFAULT_COLOR;
}
