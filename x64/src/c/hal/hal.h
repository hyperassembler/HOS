#ifndef _HAL_HAL_H_
#define _HAL_HAL_H_
#include "../common/sys/kdef.h"
#include "../common/sys/type.h"
#include "multiboot.h"
#include "../common/sys/sys_info.h"

//concurrency
extern void _KERNEL_ABI hal_spin_lock(uint32_t * lock);
extern void _KERNEL_ABI hal_spin_unlock(uint32_t * lock);

// loaded kernel addr
extern char kernel_start[];
extern char kernel_end[];

//hal
boot_info_t*_KERNEL_ABI hal_init(multiboot_info_t* m_info);

//debug
extern void _KERNEL_ABI BOCHS_MAGIC_BREAKPOINT();

#endif
