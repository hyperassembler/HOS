HAL_KERNEL_BASE_PADDR equ 0x1000000 ; #16 MB. The current setup requires the kernel to be loaded to the first 1GB
global HAL_KERNEL_BASE_PADDR

HAL_KERNEL_BASE_VADDR equ 0xFFFFFFFF80000000
global HAL_KERNEL_BASE_VADDR

%define GET_PADDR(x) ((x) - HAL_KERNEL_BASE_VADDR)