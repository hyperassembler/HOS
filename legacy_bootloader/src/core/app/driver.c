#include "bifrost.h"

static int count = 0;
void timer_proc(void *kp, void *up)
{
    hw_printf("SYS_TICK: %d . COUNT: %d.\n", ke_get_system_tick(), ke_interlocked_increment(&count, 1));
}

hw_handle_t event;

void driver2(void* par)
{
    hw_printf("Sleeping...Thread2\n");
    hw_thread_sleep(5);
    hw_printf("Signaling event..\n");
    hw_event_signal(event);
    hw_printf("Sleeping2...Thread2\n");
    hw_thread_sleep(5);
    hw_printf("Exiting..\n");
    hw_thread_exit(10);
}

void driver(void *par)
{
    hw_handle_t timer;

    hw_timer_create(&timer, TIMER_TYPE_AUTO_RESET);
    hw_timer_set(timer, 2, true);

    while (count < 20)
    {
        timer_proc(NULL, NULL);
        hw_timer_wait(timer);
    }

    hw_timer_cancel(timer);
    hw_close_handle(timer);

    hw_printf("Sleeping for some cycles..\n");
    hw_thread_sleep(10);
    hw_printf("Hmmm... sound sleep...\n");
    if(ke_get_current_core() == 1)
    {
        hw_printf("Core1 finished...\n");

    }
    else
    {
        hw_event_create(&event, EVENT_TYPE_MANUAL);
        hw_handle_t thread2_handle;
        hw_thread_create(driver2, NULL, PRIORITY_DEFAULT, THREAD_DEFAULT_STACK_SIZE, &thread2_handle);
        hw_thread_start(thread2_handle);
        hw_printf("Waiting for event...\n");
        hw_event_wait(event);
        hw_printf("Waiting for driver2 exit...\n");
        hw_wait_for_thread_exit(thread2_handle);
        int32_t exit;
        hw_thread_get_exit_code(thread2_handle, &exit);
        hw_printf("Thread2 exited with %d\n", exit);
        hw_close_handle(thread2_handle);
        hw_close_handle(event);
    }
    hw_thread_exit(0);
}
