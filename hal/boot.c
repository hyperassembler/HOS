#include "hal/print.h"
#include "hal/mem.h"
#include "hal/intr.h"
#include "hal/cpu.h"
#include "lib/sxtdlib.h"
#include "hal/boot.h"
#include "status.h"

static void SXAPI halp_obtain_cpu_info(struct boot_info *hal_info)
{
	if (hal_info == NULL)
	{
		return;
	}
	uint32 eax = 0, ebx = 0, ecx = 0, edx = 0;
	hal_cpuid(&eax, &ebx, &ecx, &edx);
	lb_mem_copy(&ebx, &hal_info->cpu_vd_str[0], sizeof(uint32));
	lb_mem_copy(&edx, &hal_info->cpu_vd_str[4], sizeof(uint32));
	lb_mem_copy(&ecx, &hal_info->cpu_vd_str[8], sizeof(uint32));
	hal_info->cpu_vd_str[12] = 0;
}

sx_status SXAPI hal_init(void *m_info)
{
	if (m_info == NULL || (uint64) m_info & lb_bit_field_mask(0, 2))
	{
		return STATUS_FAIL;
	}

	// init HAL infrastructures
	hal_print_init();
	hal_mem_init();


	struct boot_info *boot_info = halloc(sizeof(struct boot_info));

	// obtain cpu info
	halp_obtain_cpu_info(boot_info);

	// init interrupt
	if (hal_interrupt_init() != 0)
	{
		return STATUS_FAIL;
	}

	return STATUS_SUCCESS;
}
