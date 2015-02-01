OUTPUT_FORMAT(binary)
SECTIONS
{
  .text.start(0x1000000) :
  {
    *(.entry)
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
