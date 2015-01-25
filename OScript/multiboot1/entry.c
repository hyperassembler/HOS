#define HYPKERNEL32 __attribute__((__cdecl__))
extern void* kernel_stack;
extern void print_str(char* dst);
void HYPKERNEL32 hk_main(void* multiboot_info)
{
	char* str = "Kernel is here";
	print_str(str);
    fuck:
        goto fuck;
}
