SECTIONS
{
  . = 0x1000000;
  kernel_start = .;

  .entry : ALIGN(0x1000)
  {
    *(.entry)
  }

  .text : ALIGN(0x1000)
  {
    *(.text)
  }

  .data : ALIGN(0x1000)
  {
    *(.data)
    *(.rodata)
  }

  .bss : ALIGN(0x1000)
  {
    *(.bss)
  }

  kernel_end = .;
}
