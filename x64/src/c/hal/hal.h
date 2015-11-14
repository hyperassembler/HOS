#ifndef _HAL_HAL_H_
#define _HAL_HAL_H_
#include "../common/sys/kdef.h"
#include "../common/sys/type.h"
#include "multiboot.h"
#include "../common/sys/sys_info.h"

// concurrency
void _KERNEL_ABI hal_spin_lock(uint64_t * lock);
void _KERNEL_ABI hal_spin_unlock(uint64_t * lock);

// Atomically set *dst = val
// return: the previous value of *dst
extern uint64_t _KERNEL_ABI hal_interlocked_exchange(_IN _OUT uint64_t* dst,
                                                     _IN uint64_t val);
// loaded kernel addr
extern char kernel_start[];
extern char kernel_end[];

//hal
boot_info_t*_KERNEL_ABI hal_init(multiboot_info_t* m_info);

//debug
extern void _KERNEL_ABI BOCHS_MAGIC_BREAKPOINT();

#endif
