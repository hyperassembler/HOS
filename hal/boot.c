#include "print.h"
#include "mem.h"
#include "intr.h"
#include "cpu.h"
#include "call.h"
#include "hal_export.h"

//static void
//halp_obtain_cpu_info(struct boot_info *hal_info)
//{
//    if (hal_info == NULL)
//    {
//        return;
//    }
//    uint32 eax = 0, ebx = 0, ecx = 0, edx = 0;
//    hal_cpuid(&eax, &ebx, &ecx, &edx);
//    mem_cpy(&ebx, &hal_info->cpu_vd_str[0], sizeof(uint32));
//    mem_cpy(&edx, &hal_info->cpu_vd_str[4], sizeof(uint32));
//    mem_cpy(&ecx, &hal_info->cpu_vd_str[8], sizeof(uint32));
//    hal_info->cpu_vd_str[12] = 0;
//}

void HABI
hal_main(void *m_info);

void HABI
hal_main(void *m_info)
{
    if (m_info == NULL || (uint64) m_info & bit_field_mask(0, 2))
    {
        hal_halt_cpu();
    }

    // init HAL infrastructures
    hal_print_init();
    hal_mem_init();


    struct boot_info *boot_info = halloc(sizeof(struct boot_info));

//    // obtain cpu info
//    halp_obtain_cpu_info(boot_info);

    // init interrupt
    if (hal_interrupt_init() != 0)
    {
        hal_halt_cpu();
    }

    ke_main(boot_info);
}
