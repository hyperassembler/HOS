OUTPUT_FORMAT("binary")
INPUT
(
	multiboot.o
	entry.o
)
OUTPUT(kernel.bin)
SECTIONS
{
  .text.start (0x1000000):
  {
    multiboot.o(.multiboot)
  }

  .text :
  {
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
