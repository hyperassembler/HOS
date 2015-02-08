OUTPUT_FORMAT(binary)
SECTIONS
{
  . = 0x100000;
  kernel_start = .;

  .entry :
  {
    *(.entry)
  }

  .text :
  {
    *(.text)
  }

  .data :
  {
    *(.data)
    *(.rodata)
  }

  .bss :
  {
    *(.bss)
  }

  .stack :
  {
    *(.stack)
  }

  .heap :
  {
    *(.heap)
  }

  kernel_end = .;
}
