#ifndef _KERNEL_HAL_MEMDEF_H_
#define _KERNEL_HAL_MEMDEF_H_
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

#define KERNEL_IMAGE_PADDR       (0x1000000)
#define KERNEL_PAGE_SIZE         (0x1000)

#define KERNEL_SPACE_VADDR       (0xFFFF800000000000)
#define KERNEL_RESERVED_VADDR    KERNEL_SPACE_VADDR
#define KERNEL_RESERVED_SIZE     (0x00007F0000000000)
#define KERNEL_PAGE_TABLE_VADDR  (0xFFFFFF0000000000)
#define KERNEL_PAGE_TABLE_SIZE   (0x0000008000000000)
#define KERNEL_DYNAMIC_VADDR     (0xFFFFFF8000000000)
#define KERNEL_DYNAMIC_SIZE      (0x0000007F80000000)
#define KERNEL_IMAGE_VADDR       (0xFFFFFFFF80000000)
#define KERNEL_IMAGE_SIZE        (0x0000000080000000)
#endif