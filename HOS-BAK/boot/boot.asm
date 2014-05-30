org  07c00h

BaseOfStack		equ	07c00h	
BaseOfLoader		equ	09000h	
OffsetOfLoader		equ	0100h	
	jmp short START		
	%include "FAT12Header.inc"

START:	
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack
	;FAT12ReadFile(ULONG16 Base,ULONG16 Offset,ULONG16 OffsetFileName)
	push LoaderFileName
	push OffsetOfLoader
	push BaseOfLoader
	call near FAT12ReadFile
	add sp,6
	jmp BaseOfLoader:OffsetOfLoader
	%include "fat12readfile.lib"
LoaderFileName: db 'LOADER  BIN',0
times 	510-($-$$)	db	0
dw 	0xaa55			
