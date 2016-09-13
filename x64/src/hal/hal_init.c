/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "hal_print.h"
#include "hal_mem.h"
#include "hal_intr.h"
#include "hal_arch.h"
#include "k_stdlib.h"
#include "s_boot.h"

extern char kernel_start[];
extern char kernel_end[];

static void KAPI _hal_obtain_cpu_info(k_hal_boot_info_t *hal_info)
{
    if(hal_info == NULL)
        return;
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    hal_cpuid(&eax,&ebx,&ecx,&edx);
    ke_mem_copy(&ebx, &hal_info->cpu_vd_str[0], sizeof(uint32_t));
    ke_mem_copy(&edx, &hal_info->cpu_vd_str[4], sizeof(uint32_t));
    ke_mem_copy(&ecx, &hal_info->cpu_vd_str[8], sizeof(uint32_t));
    hal_info->cpu_vd_str[12] = 0;
}

void KAPI hal_main(void *m_info)
{
    if (m_info == NULL || (uint64_t) m_info & ke_bit_field_mask(0, 2))
        return;

    // init HAL infrastructures
    hal_print_init();
    hal_mem_init();


    k_hal_boot_info_t* boot_info = halloc(sizeof(k_hal_boot_info_t));

    // set up HAL def
    boot_info->krnl_start = (uint64_t)kernel_start;
    boot_info->krnl_end = (uint64_t)kernel_end;

    // obtain cpu info
    _hal_obtain_cpu_info(boot_info);

    // init interrupt
    if(hal_interrupt_init() != 0)
    {
        return;
    }
    // pass the control to the kernel
    ke_main(boot_info);
    return;
}
