#ifndef _HAL_HAL_H_
#define _HAL_HAL_H_
#include "../common/kdef.h"
#include "../common/type.h"
#include "multiboot.h"

//concurrency
extern void NATIVE64 hal_spin_lock(uint32_t * lock);
extern void NATIVE64 hal_spin_unlock(uint32_t * lock);

//inti
void NATIVE64 hal_init(multiboot_info_t* m_info);

//debug
extern void NATIVE64 HLT_CPU(void);
extern void NATIVE64 BOCHS_MAGIC_BREAKPOINT();

#endif
