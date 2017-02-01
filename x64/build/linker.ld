SECTIONS
{
  . = 0xFFFFFFFF80000000;

  kernel_start = .;

  .entry ALIGN(0x1000) : AT(ADDR(.entry) - 0xFFFFFFFF80000000)
  {
    *(.entry)
  }

  .text ALIGN(0x1000) : AT(ADDR(.text) - 0xFFFFFFFF80000000)
  {
    *(.text)
  }

  .data ALIGN(0x1000) : AT(ADDR(.data) - 0xFFFFFFFF80000000)
  {
    *(.data)
    *(.rodata)
  }

  .bss ALIGN(0x1000) : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
  {
    *(.bss)
  }

  .heap ALIGN(0x1000) : AT(ADDR(.heap) - 0xFFFFFFFF80000000)
  {
    *(.heap)
  }

  .stack ALIGN(0x1000) : AT(ADDR(.stack) - 0xFFFFFFFF80000000)
  {
    *(.stack)
  }

  kernel_end = .;
}
