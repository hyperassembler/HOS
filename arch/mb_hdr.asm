#define ASM_FILE
#include "multiboot2.h"
#include <arch/mlayout.h>

ASM_MULTIBOOT_CHECK_SUM equ (0xFFFFFFFF - (MULTIBOOT2_HEADER_MAGIC + ASM_MULTIBOOT_HEADER_SIZE + MULTIBOOT_ARCHITECTURE_I386) + 1)

section .multiboot_header
bits 32
align 0x1000
;====================
align MULTIBOOT_HEADER_ALIGN
start_hdr:
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT_ARCHITECTURE_I386
    dd ASM_MULTIBOOT_HEADER_SIZE
    dd ASM_MULTIBOOT_CHECK_SUM
;====================
align MULTIBOOT_INFO_ALIGN
    dw MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST
    dw 0 ; flag
    dd (8+4*3) ; size
    dd MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME
    dd MULTIBOOT_TAG_TYPE_MMAP
    dd MULTIBOOT_TAG_TYPE_ACPI_NEW
;====================
align MULTIBOOT_INFO_ALIGN
    dw MULTIBOOT_HEADER_TAG_FRAMEBUFFER; type=5
    dw 0 ; flag
    dw 20 ; size
    dd 0 ; width
    dd 0 ; depth
    dd 0 ; height
;====================
align MULTIBOOT_INFO_ALIGN
    dw MULTIBOOT_HEADER_TAG_MODULE_ALIGN; type=6
    dw 0 ; flag
    dd 8 ; size
;====================
align MULTIBOOT_INFO_ALIGN
    dw MULTIBOOT_HEADER_TAG_END
    dw 0 ; flag
    dd 8 ; size
;====================
ASM_MULTIBOOT_HEADER_SIZE equ ($ - start_hdr)
