#include "bifrost_alloc.h"
#include "bifrost_print.h"
#include "bifrost_thread.h"
#include "bifrost_dpc.h"
#include "bifrost_boot.h"
#include "bifrost_apc.h"
#include "bifrost_timer.h"

extern void driver(void *par);

hw_arch_bootinfo_t info;

_Bool global_init_finished = false;

int kmain(void)
{
    hw_handle_t driver_handle;

    ke_hal_setup(&info);
    hw_printf("Initializing...\n");

    uint32_t coreid = ke_get_current_core();

    // global inits only need to be done on one core
    if(coreid == 0)
    {
        hw_alloc_setup();
        ke_reference_setup();

        global_init_finished = true;
    }

    while(!global_init_finished);

    ke_lower_irql(HW_IRQL_USER_LEVEL);

    // core specific stuff init
    hw_thread_setup();
    ke_apc_setup(info.int_info.apc_vec);
    ke_dpc_setup(info.int_info.dpc_vec);
    ke_timer_setup();
    ke_register_intr_handler(info.int_info.timer_vec, ke_timer_interrupt_handler, NULL);

    hw_printf("Initialization completed.\n");
    hw_thread_create(driver, NULL, PRIORITY_DEFAULT, THREAD_DEFAULT_STACK_SIZE, &driver_handle);
    hw_thread_start(driver_handle);
    ke_set_timer_timeout(1);

    while(1);
}

