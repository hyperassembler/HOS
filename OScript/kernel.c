#include "grub.h"
#include "type.h"
#define HKAPI32 __cdecl
#define HKAPI64
extern PBYTE _KERNEL_STACK;
extern void printf(char* dst);
void HKAPI32 HkKernelEntry(PMULTIBOOT_TAG multiboot_info) 
{
	char[] str = "Kernel is here";
	printf(&str[0]);
}