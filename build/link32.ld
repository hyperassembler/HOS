OUTPUT_FORMAT(binary)
SECTIONS
{
  .text.start(0x1000000) :
  {
    *(.multiboot)
    *(.text)
  }

  .data :
  {
    *(.data)
  }

  .bss :
  {
    *(.bss)
  }
}
