#ifndef _HAL_HAL_H_
#define _HAL_HAL_H_
#include "../common/sys/kdef.h"
#include "../common/sys/type.h"
#include "multiboot.h"
#include "../common/sys/sys_info.h"

//concurrency
extern void NATIVE64 hal_spin_lock(uint32_t * lock);
extern void NATIVE64 hal_spin_unlock(uint32_t * lock);

//inti
boot_info_t* NATIVE64 hal_init(multiboot_info_t* m_info);

//debug
extern void NATIVE64 HLT_CPU(void);
extern void NATIVE64 BOCHS_MAGIC_BREAKPOINT();

#endif
