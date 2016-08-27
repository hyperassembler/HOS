#include <bifrost_thread.h>
#include <bifrost_ref.h>
#include <bifrost_dpc.h>
#include <bifrost_stdlib.h>
#include <bifrost_print.h>
#include "bifrost_mem.h"
#include "bifrost_thread.h"
#include "bifrost_test.h"

static hw_spin_lock_t main_core_lock = HW_LOCK_INITIALIZER;
static uint32_t main_core = 0xFFFFFFFF;
static uint32_t main_core_done = 0;

static void delay(uint32_t dum)
{
    while (dum--);
}

static void determine_main_core()
{
    ke_spin_lock(&main_core_lock);
    if (main_core == 0xFFFFFFFF)
    {
        main_core = ke_get_current_core();
    }
    ke_spin_unlock(&main_core_lock);
}

static void dummy_thread(void *args)
{
    while(1);
}

// main core case
static hw_result_t thread_test_create_destroy_open()
{

    hw_result_t status = STATUS_SUCCESS;
    hw_handle_t handle = 0, open_handle = 0;
    status = hw_thread_create(dummy_thread,
                     NULL,
                     PRIORITY_DEFAULT,
                     THREAD_DEFAULT_STACK_SIZE,
                     &handle);
    int32_t exit_code = 0;
    hw_ref_node_t* ref;
    hw_open_obj_by_handle(handle, &ref);
    hw_tcb_t* tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);

    if(HW_SUCCESS(status))
    {
        status = hw_thread_open(tcb->thread_id, &open_handle);
    }

    ke_dereference_obj(&tcb->ref_node);

    if(HW_SUCCESS(status))
    {
        status = hw_thread_terminate(handle);
    }

    if(HW_SUCCESS(status))
    {
        status = ke_thread_yield(ke_get_current_core());
    }

    if(HW_SUCCESS(status))
    {
        status = hw_thread_get_exit_code(handle, &exit_code);
    }

    if(HW_SUCCESS(status))
    {
        status = exit_code == (int32_t)THREAD_EXIT_CODE_TERMINATED ? STATUS_SUCCESS : THREAD_STATUS_INVALID_STATE;
    }

    if(HW_SUCCESS(status))
    {
        status = hw_close_handle(handle);
    }

    if(HW_SUCCESS(status))
    {
        status = hw_close_handle(open_handle);
    }

    return status;
}


static hw_result_t thread_test_block_resume_terminate_same_core()
{
    hw_result_t status = STATUS_SUCCESS;
    hw_handle_t handle;
    status = hw_thread_create(dummy_thread,
                              NULL,
                              PRIORITY_DEFAULT,
                              THREAD_DEFAULT_STACK_SIZE,
                              &handle);

    if(HW_SUCCESS(status))
    {
        hw_thread_start(handle);
        status = ke_thread_yield(ke_get_current_core());
    }

    // now block
    if(HW_SUCCESS(status))
    {
        hw_thread_block(handle);
        status = ke_thread_yield(ke_get_current_core());
    }

    // assert state
    if(HW_SUCCESS(status))
    {
        status = hw_thread_assert_state(handle, STATE_BLOCK);
    }

    // now resume
    if(HW_SUCCESS(status))
    {
        hw_thread_resume(handle);
        status = ke_thread_yield(ke_get_current_core());
    }

    // assert state
    if(HW_SUCCESS(status))
    {
        status = hw_thread_assert_state(handle, STATE_READY);
    }

    // now terminate
    if(HW_SUCCESS(status))
    {
        hw_thread_terminate(handle);
        status = ke_thread_yield(ke_get_current_core());
    }

    // assert state
    if(HW_SUCCESS(status))
    {
        status = hw_thread_assert_state(handle, STATE_EXIT);
    }

    // clean up
    if(HW_SUCCESS(status))
    {
        status = hw_thread_get_exit_code(handle, NULL);
    }

    if(HW_SUCCESS(status))
    {
        status = hw_close_handle(handle);
    }

    return status;
}

////// multi-core tests

static void thread_test_block_resume_driver(void* arg)
{
    hw_handle_t slave_handle = (hw_handle_t)arg;
    hw_result_t status = STATUS_SUCCESS;

    hw_ref_node_t* ref;
    hw_open_obj_by_handle(slave_handle, &ref);
    hw_tcb_t* tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);

    // now block
    if(HW_SUCCESS(status))
    {
        hw_thread_block(slave_handle);
        status = ke_thread_yield(tcb->core_id);
        delay(10000);
    }

    // assert state
    if(HW_SUCCESS(status))
    {
        status = hw_thread_assert_state(slave_handle, STATE_BLOCK);
    }

    // now resume
    if(HW_SUCCESS(status))
    {
        hw_thread_resume(slave_handle);
        status = ke_thread_yield(tcb->core_id);
        delay(10000);
    }

    // assert state
    if(HW_SUCCESS(status))
    {
        status = hw_thread_assert_state(slave_handle, STATE_RUN);
    }

    // now terminate
    if(HW_SUCCESS(status))
    {
        hw_thread_terminate(slave_handle);
        status = ke_thread_yield(tcb->core_id);
        delay(10000);
    }

    // assert state
    if(HW_SUCCESS(status))
    {
        status = hw_thread_assert_state(slave_handle, STATE_EXIT);
    }

    ke_dereference_obj(&tcb->ref_node);

    hw_thread_exit(status);
}

static hw_handle_t driver = 0;
static hw_handle_t slave = 0;

static hw_result_t thread_test_block_resume_terminate_other_core()
{
    uint32_t coreid = ke_get_current_core();
    // driver thread
    hw_result_t status = STATUS_SUCCESS;
    if(coreid == main_core)
    {
        while(slave == 0);
        status = hw_thread_create(thread_test_block_resume_driver,
                                  (void*)slave,
                                  PRIORITY_DEFAULT,
                                  THREAD_DEFAULT_STACK_SIZE,
                                  &driver);
    }
    else
    {
        // dummy thread
        status = hw_thread_create(dummy_thread,
                                  NULL,
                                  PRIORITY_DEFAULT,
                                  THREAD_DEFAULT_STACK_SIZE,
                                  &slave);
        while(driver == 0);

    }
    delay(10000);

    hw_handle_t self = coreid == main_core ? driver : slave;

    if(HW_SUCCESS(status))
    {
        hw_thread_start(self);
    }

    if(HW_SUCCESS(status))
    {
        status = hw_wait_for_thread_exit(self);
    }

    if(HW_SUCCESS(status))
    {
        if (coreid == main_core)
        {
            int32_t exitcode;
            status = hw_thread_get_exit_code(self, &exitcode);
            if (HW_SUCCESS(status))
            {
                status = (hw_result_t)exitcode;
            }
        }
    }

    return status;
}

void thread_test_main()
{
    //determine the main core id
    determine_main_core();
    if(ke_get_current_core() == main_core)
        hw_start_test("Bifrost Threading Test");


    if (ke_get_current_core() == main_core)
    {
        hw_run_case("thread_test_create_destroy_open", thread_test_create_destroy_open());
        hw_run_case("thread_test_block_resume_terminate_same_core", thread_test_block_resume_terminate_same_core());
        main_core_done = 1;
    }
    else
    {
        while (main_core_done == 0);
    }

    hw_run_case("thread_test_block_resume_terminate_other_core",thread_test_block_resume_terminate_other_core());

    if(ke_get_current_core() == main_core)
        hw_end_test();
}
