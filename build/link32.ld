OUTPUT_FORMAT(binary)
SECTIONS
{
  .text.start(0x100000) :
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
