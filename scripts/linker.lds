#define ASM_FILE
#include <arch/mlayout.h>

ENTRY(arch_init_32)

SECTIONS
{
	. = KERN_BASE_START + KERN_IMG_PADDR;

    KERN_IMG_START = .;

	.multiboot_header ALIGN(0x1000) : AT(ADDR(.multiboot_header) - KERN_BASE_START)
	{
		*(.multiboot_header)
	}

    .text ALIGN(0x1000) : AT(ADDR(.text) - KERN_BASE_START)
    {
        *(.text)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - KERN_BASE_START)
    {
        *(.data)
        *(.rodata*)
    }

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - KERN_BASE_START)
    {
        *(.bss)
        *(COMMON)
    }

    ARCH_ML_KIMAGE_STOP = .;
}
