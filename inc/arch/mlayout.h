#pragma once

/**
 * Kernel Memory Layout
 * ----------------------- 0x0000,0000,0000,0000 - User Space
 * Application       SIZE: 0x0000,8000,0000,0000 (256x PML4)
 * ----------------------- 0x0000,8000,0000,0000
 * Non-canonical
 * ----------------------- 0xFFFF,8000,0000,0000 - Kernel Space
 * PMAP              SIZE: 0x0000,4000,0000,0000 (128x PML4)
 * ----------------------- 0xFFFF,C000,0000,0000
 * Kernel Dynamic    SIZE: 0x0000,3000,0000,0000
 * ----------------------- 0xFFFF,F000,0000,0000
 * Unused
 * ----------------------- 0xFFFF,FFFF,8000,0000
 * Kernel Image      SIZE: 0x0000,0000,8000,0000 (2GB)
 * ----------------------- 0xFFFF,FFFF,FFFF,FFFF
**/

/**
 * kernel loaded at physical address 16MB
 * 0x1000000 = 16777216 = 16 * 1024 * 1024
 */
#define KERNEL_IMG_PADDR       (0x1000000)
#define KERNEL_PAGE_SIZE         (0x1000)


#define KERNEL_SPACE_VADDR       (0xFFFF800000000000)
#define KERNEL_PMAP_VADDR        (0xFFFF800000000000)
#define KERNEL_PMAP_SIZE         (0x0000400000000000)
#define KERNEL_DYN_VADDR         (0xFFFFFF8000000000)
#define KERNEL_DYN_SIZE          (0x0000300000000000)
#define KERNEL_IMG_VADDR         (0xFFFFFFFF80000000)
#define KERNEL_IMG_SIZE          (0x000000007FFFFFFF)

