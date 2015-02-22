global read_port
global write_port
[SECTION .text]
[BITS 32]
write_port:
	mov	edx, [esp + 4]
	mov	al, [esp + 4 + 4]
	out	dx, al
	nop
	nop
	ret

read_port:
	mov	edx, [esp + 4]
	xor	eax, eax
	in	al, dx
	nop
	nop
	ret