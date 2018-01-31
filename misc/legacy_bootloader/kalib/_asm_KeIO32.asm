global __asm_KeWritePort;VOID  _asm_KeWritePort(ULONG16 Port,ULONG8 Value);
global __asm_KeReadPort;ULONG8 _asm_KeReadPort(ULONG16 Port);
global __asm_KeLoadIDT;VOID _asm_KeLoadIDT(VOID)
global __asm_KeWriteGDT; VOID _asm_KeWriteGDT(PGDT_DESCRIPTOR DescBase ,PWrite_GDT_DESCRIPTOR PDescriptor, ULONG32 index);
;IDT DESC
global __asm_KeDivideError
global __asm_KeSingleStepException
global __asm_KeNMI
global __asm_KeBreakpointException
global __asm_KeOverflow
global __asm_KeBoundsCheck
global __asm_KeInvalidOpcode
global __asm_KeCoprNotAvailable
global __asm_KeDoubleFault
global __asm_KeCoprSegmentOverrun
global __asm_KeInvalidTSS
global __asm_KeSegmentNotPresent
global __asm_KeStackException
global __asm_KeGeneralProtection
global __asm_KePageFault
global __asm_KeCoprError
;8259A Interrupts
global __asm_KeIrqInt00
global __asm_KeIrqInt01
global __asm_KeIrqInt02
global __asm_KeIrqInt03
global __asm_KeIrqInt04
global __asm_KeIrqInt05
global __asm_KeIrqInt06
global __asm_KeIrqInt07
global __asm_KeIrqInt08
global __asm_KeIrqInt09
global __asm_KeIrqInt10
global __asm_KeIrqInt11
global __asm_KeIrqInt12
global __asm_KeIrqInt13
global __asm_KeIrqInt14
global __asm_KeIrqInt15

global __asm_EIP2EAX

extern _KeIrqIntHandler
extern _KeIDTPtr
extern _KeExceptionHandler

SLCT_FLAT_C equ 16

	 
	

__asm_EIP2EAX:
	mov eax, [esp]
	ret

__asm_KeReadPort:
	mov	edx, [esp + 4]
	xor	eax, eax
	in	al, dx
	nop	
	nop
	ret
;push ebp
;mov ebp,esp
;push edx
;mov edx,[ss:ebp+8]
;xor eax,eax
;in al,dx
;nop 
;nop
;nop
;pop edx
;pop ebp
;ret

__asm_KeWritePort:
	mov	edx, [esp + 4]	
	mov	al, [esp + 4 + 4]	
	out	dx, al
	nop	
	nop
	ret
	
;push ebp
;mov ebp,esp
;push edx
;push eax
;mov edx,[ss:ebp+8]
;mov eax,[ss:ebp+12]
;out dx,al
;nop
;nop
;nop
;pop eax
;pop edx
;ret

__asm_KeLoadIDT:
lidt [_KeIDTPtr]
jmp SLCT_FLAT_C:.force
.force:
sti
ret

__asm_KeDivideError:
push 0xFFFFFFFF
push 0
jmp exception

__asm_KeSingleStepException:
push 0xFFFFFFFF
push 1
jmp exception

__asm_KeNMI:
push 0xFFFFFFFF
push 2
jmp exception

__asm_KeBreakpointException:
push 0xFFFFFFFF
push 3
jmp exception

__asm_KeOverflow:
push 0xFFFFFFFF
push 4
jmp exception

__asm_KeBoundsCheck:
push 0xFFFFFFFF
push 5
jmp exception

__asm_KeInvalidOpcode:
push 0xFFFFFFFF
push 6
jmp exception

__asm_KeCoprNotAvailable:
push 0xFFFFFFFF
push 7
jmp exception

__asm_KeDoubleFault:
push 0xFFFFFFFF
push 8
jmp exception

__asm_KeCoprSegmentOverrun:
push 0xFFFFFFFF
push 9 
jmp exception

__asm_KeInvalidTSS:
push 0xFFFFFFFF
push 10
jmp exception

__asm_KeSegmentNotPresent:
push 0xFFFFFFFF
push 11
jmp exception

__asm_KeStackException:
push 0xFFFFFFFF
push 12
jmp exception

__asm_KeGeneralProtection:
push 0xFFFFFFFF
push 13
jmp exception

__asm_KePageFault:
push 0xFFFFFFFF
push 14
jmp exception

__asm_KeCoprError:
push 0xFFFFFFFF
push 15
jmp exception

exception:
call _KeExceptionHandler
add esp,8
hlt

__asm_KeIrqInt00:
push 0
jmp interrupt

__asm_KeIrqInt01:
push 1
jmp interrupt

__asm_KeIrqInt02:
push 2
jmp interrupt

__asm_KeIrqInt03:
push 3
jmp interrupt

__asm_KeIrqInt04:
push 4
jmp interrupt

__asm_KeIrqInt05:
push 5
jmp interrupt

__asm_KeIrqInt06:
push 6
jmp interrupt

__asm_KeIrqInt07:
push 7
jmp interrupt

__asm_KeIrqInt08:
push 8
jmp interrupt

__asm_KeIrqInt09:
push 9
jmp interrupt

__asm_KeIrqInt10:
push 10
jmp interrupt

__asm_KeIrqInt11:
push 11
jmp interrupt

__asm_KeIrqInt12:
push 12
jmp interrupt

__asm_KeIrqInt13:
push 13
jmp interrupt

__asm_KeIrqInt14:
push 14
jmp interrupt

__asm_KeIrqInt15:
push 15
jmp interrupt

interrupt:
call _KeIrqIntHandler
add esp,4
hlt 

