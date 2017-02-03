OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(hal_entry_32)

SECTIONS
{
  . = HAL_KERNEL_BASE_VADDR + HAL_KERNEL_BASE_PADDR;

  HAL_KERNEL_START_VADDR = .;

  .multiboot_header : AT(ADDR(.multiboot_header) - HAL_KERNEL_BASE_VADDR)
  {
    *(.multiboot_header)
  }

  .text ALIGN(0x1000) : AT(ADDR(.text) - HAL_KERNEL_BASE_VADDR)
  {
    *(.text)
  }

  .data ALIGN(0x1000) : AT(ADDR(.data) - HAL_KERNEL_BASE_VADDR)
  {
    *(.data)
    *(.rodata*)
  }

  .bss ALIGN(0x1000) : AT(ADDR(.bss) - HAL_KERNEL_BASE_VADDR)
  {
    *(.bss)
    *(COMMON)
  }

  HAL_KERNEL_END_VADDR = .;

    /DISCARD/ :
	{
		*(.gcc_except_table)
		*(.eh_frame)
		*(.note)
		*(.comment)
		*(.rel.*)
		*(.rela.*)
	}
}
