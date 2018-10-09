#pragma once

/**
 * Kernel Memory Layout
 * ----------------------- 0x0000,0000,0000,0000 - User Space
 * Application       SIZE: 0x0000,8000,0000,0000 (256x PML4)
 * ----------------------- 0x0000,8000,0000,0000
 * Non-canonical
 * ----------------------- 0xFFFF,8000,0000,0000 - Kernel Space
 * Reserved          SIZE: 0x0000,7F00,0000,0000 (254x PML4)
 * ----------------------- 0xFFFF,FF00,0000,0000
 * Page Table        SIZE: 0x0000,0080,0000,0000 (1x PML4)
 * ----------------------- 0xFFFF,FF80,0000,0000
 * Kernel Dynamic    SIZE: 0x0000,007F,8000,0000 (Kernel Dynamic + Kernel Image = 1x PML4)
 * ----------------------- 0xFFFF,FFFF,8000,0000
 * Kernel Image      SIZE: 0x0000,0000,8000,0000
 * ----------------------- 0xFFFF,FFFF,FFFF,FFFF
**/

/**
 * kernel loaded at physical address 16MB
 * 0x1000000 = 16777216 = 16 * 1024 * 1024
 */
#define KERNEL_IMAGE_PADDR       (0x1000000)
#define KERNEL_PAGE_SIZE         (0x1000)


#define KERNEL_SPACE_VADDR       (0xFFFF800000000000)
#define KERNEL_RESERVED_VADDR    KERNEL_SPACE_VADDR
#define KERNEL_PAGE_TABLE_VADDR  (0xFFFFFF0000000000)
#define KERNEL_DYNAMIC_VADDR     (0xFFFFFF8000000000)

/**
 * Minus 2GB
 */
#define KERNEL_IMAGE_VADDR       (0xFFFFFFFF80000000)
#define KERNEL_IMAGE_OFFSET      (KERNEL_IMAGE_PADDR)

