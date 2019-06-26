#define ASM_FILE
#include <arch/mlayout.h>

ENTRY(arch_init_32)

SECTIONS
{
	. = KERNEL_IMG_VADDR + KERNEL_IMG_PADDR;

	.multiboot_header ALIGN(KERNEL_PAGE_SIZE) : AT(ADDR(.multiboot_header) - KERNEL_IMG_VADDR)
	{
		*(.multiboot_header)
	}

	.text ALIGN(KERNEL_PAGE_SIZE) : AT(ADDR(.text) - KERNEL_IMG_VADDR)
	{
		*(.text)
	}

	.data ALIGN(KERNEL_PAGE_SIZE) : AT(ADDR(.data) - KERNEL_IMG_VADDR)
	{
		*(.data)
		*(.rodata*)
	}

	.bss ALIGN(KERNEL_PAGE_SIZE) : AT(ADDR(.bss) - KERNEL_IMG_VADDR)
	{
		*(.bss)
		*(COMMON)
	}
}
