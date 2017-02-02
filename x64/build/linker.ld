OUTPUT_FORMAT(elf64-x86-64)
ENTRY(HAL_ENTRY32_PADDR)
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
  . = HAL_KERNEL_BASE_VADDR + HAL_KERNEL_BASE_PADDR;

  kernel_start = .;

  .multiboot_header ALIGN(0x1000) : AT(ADDR(.multiboot_header) - HAL_KERNEL_BASE_VADDR)
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

  kernel_end = .;
}
