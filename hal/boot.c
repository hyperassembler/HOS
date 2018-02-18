#include "hal/print.h"
#include "hal/mem.h"
#include "hal/intr.h"
#include "hal/cpu.h"
#include "lib/sxtdlib.h"
#include "hal/boot.h"
#include "status.h"

static void KABI halp_obtain_cpu_info(boot_info_t *hal_info)
{
    if(hal_info == NULL)
        return;
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    hal_cpuid(&eax,&ebx,&ecx,&edx);
    lb_mem_copy(&ebx, &hal_info->cpu_vd_str[0], sizeof(uint32_t));
    lb_mem_copy(&edx, &hal_info->cpu_vd_str[4], sizeof(uint32_t));
    lb_mem_copy(&ecx, &hal_info->cpu_vd_str[8], sizeof(uint32_t));
    hal_info->cpu_vd_str[12] = 0;
}

status_t KABI hal_init(void *m_info)
{
    if (m_info == NULL || (uint64_t) m_info & lb_bit_field_mask(0, 2))
        return STATUS_FAIL;

    // init HAL infrastructures
    hal_print_init();
    hal_mem_init();


    boot_info_t* boot_info = halloc(sizeof(boot_info_t));

    // obtain cpu info
    halp_obtain_cpu_info(boot_info);

    // init interrupt
    if(hal_interrupt_init() != 0)
    {
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}
