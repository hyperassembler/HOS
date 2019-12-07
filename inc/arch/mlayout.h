#pragma once

/**
 * Kernel Memory Layout
 * ----------------------- 0x0000,0000,0000,0000 - User Space
 * Application       SIZE: 0x0000,8000,0000,0000 (256x PML4, 128TB)
 * ----------------------- 0x0000,8000,0000,0000
 * Non-canonical
 * ----------------------- 0xFFFF,8000,0000,0000 - Kernel Space
 * Unused
 * ----------------------- 0xFFFF,9000,0000,0000
 * PMAP              SIZE: 0x0000,0400,0000,0000 (8x PML4, 4TB)
 * ----------------------- 0xFFFF,9400,0000,0000
 * Unused
 * ----------------------- 0xFFFF,FFFF,8000,0000
 * Kernel Image      SIZE: 0x0000,0000,8000,0000 (2GB)
 * ----------------------- 0xFFFF,FFFF,FFFF,FFFF
**/

/**
 * kernel loaded at physical address 32MB
 * 0x2000000 = 2 * 16777216 = 2 * 16 * 1024 * 1024
 */
#ifdef ASM_FILE

#define KERN_IMG_PADDR       0x2000000
#define KERN_PAGE_SZ         0x1000

#define KERN_PMAP_START 0xFFFF900000000000
#define KERN_PMAP_STOP  0xFFFF940000000000
#define KERN_BASE_START 0xFFFFFFFF80000000
#define KERN_BASE_STOP  0x0000000000000000

#else

#define KERN_IMG_PADDR       (0x2000000)
#define KERN_PAGE_SZ         (0x1000)

extern const char KERN_IMG_START[];
extern const char KERN_IMG_STOP[];

#define KERN_PMAP_START (0xFFFF900000000000)
#define KERN_PMAP_STOP  (0xFFFF940000000000)
#define KERN_BASE_START (0xFFFFFFFF80000000)
#define KERN_BASE_STOP  (0x0000000000000000)

#endif
