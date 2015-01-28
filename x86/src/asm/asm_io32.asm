global hk_read_port
global hk_write_port
[SECTION .text]
[BITS 32]
hk_write_port:
	mov	edx, [esp + 4]
	mov	al, [esp + 4 + 4]
	out	dx, al
	nop
	nop
	ret

hk_read_port:
	mov	edx, [esp + 4]
	xor	eax, eax
	in	al, dx
	nop
	nop
	ret