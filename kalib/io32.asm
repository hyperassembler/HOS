global _HkWritePort32 ; void HkWritePort32(uint16 port ,uint16 data)
push ebp
mov ebp,esp
mov edx,[ ebp + 8 ]
mov eax,[ ebp + 12 ]
out dx,ax
nop
nop
mov esp,ebp
pop ebp
ret

global _HkReadPort32 ; void HkWritePort32(uint16 port ,uint16 data)
push ebp
mov ebp,esp
mov edx,[ ebp + 8]
mov eax,[ ebp + 12]
in ax,dx
nop
nop
mov esp,ebp
pop ebp
ret
