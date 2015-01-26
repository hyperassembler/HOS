extern void* kernel_stack;
extern void print_str(char* src);
int hk_main(void)
{
    char* msg = "Welcome to HYP OS 1.0";
    print_str(msg);
    loop:
        goto loop;
}