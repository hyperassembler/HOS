#pragma once

/**
 * Kernel Memory Layout
 * ----------------------- 0x0000,0000,0000,0000 - User Space
 * Application       SIZE: 0x0000,8000,0000,0000 (256x PML4, 128TB)
 * ----------------------- 0x0000,7FFF,FFFF,FFFF
 * Non-canonical
 * ----------------------- 0xFFFF,8000,0000,0000 - Kernel Space
 * Unused
 * ----------------------- 0xFFFF,9000,0000,0000
 * PMAP              SIZE: 0x0000,0400,0000,0000 (8x PML4, 4TB)
 * ----------------------- 0xFFFF,93FF,FFFF,FFFF
 * ----------------------- 0xFFFF,9400,0000,0000
 * MM_PAGE           SIZE: 0x0000,0400,0000,0000 (8x PML4, 4TB)
 * ----------------------- 0xFFFF,97FF,FFFF,FFFF
 * ----------------------- 0xFFFF,9800,0000,0000
 * RPT (recursive page tables)
 *                   SIZE: 0x0000,0080,0000,0000 (1x PML4, 512GB)
 * ----------------------- 0xFFFF,987F,FFFF,FFFF
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

#define KERN_PMAP_START     0xFFFF900000000000
#define KERN_PMAP_STOP      0xFFFF93FFFFFFFFFF
#define KERN_MM_PAGE_START  0xFFFF940000000000
#define KERN_MM_PAGE_STOP   0xFFFF97FFFFFFFFFF
#define KERN_RPT_START      0xFFFF980000000000
#define KERN_RPT_STOP       0xFFFF987FFFFFFFFF
#define KERN_BASE_START     0xFFFFFFFF80000000
#define KERN_BASE_STOP      0xFFFFFFFFFFFFFFFF
#else

#define ARCH_ML_KIMAGE_PADDR       (0x2000000u)
#define ARCH_KPAGE_SZ         (0x1000u)

extern const char ARCH_ML_KIMAGE_START[];
extern const char ARCH_ML_KIMAGE_STOP[];

#define ARCH_ML_USPACE_END (0x00007FFFFFFFFFFFu)
#define ARCH_ML_KSPACE_START    (0xFFFF800000000000u)
#define ARCH_ML_PMAP_START    (0xFFFF900000000000u)
#define ARCH_ML_PMAP_STOP     (0xFFFF93FFFFFFFFFFu)
#define ARCH_ML_MMPAGE_START  (0xFFFF940000000000u)
#define ARCH_ML_MMPAGE_STOP   (0xFFFF97FFFFFFFFFFu)
#define ARCH_ML_KBASE_START  (0xFFFFFFFF80000000u)
#define ARCH_ML_KBASE_STOP   (0xFFFFFFFFFFFFFFFFu)

#define ARCH_ML_MAX_RAM (ARCH_ML_PMAP_STOP - ARCH_ML_PMAP_START + 1)

#endif
