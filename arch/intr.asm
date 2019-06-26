%macro PUSHAQ 0
   push rax      ;save current rax
   push rbx      ;save current rbx
   push rcx      ;save current rcx
   push rdx      ;save current rdx
   push rbp      ;save current rbp
   push rdi      ;save current rdi
   push rsi      ;save current rsi
   push r8         ;save current r8
   push r9         ;save current r9
   push r10      ;save current r10
   push r11      ;save current r11
   push r12      ;save current r12
   push r13      ;save current r13
   push r14      ;save current r14
   push r15      ;save current r15
%endmacro

%macro POPAQ 0
    pop r15         ;restore current r15
    pop r14         ;restore current r14
    pop r13         ;restore current r13
    pop r12         ;restore current r12
    pop r11         ;restore current r11
    pop r10         ;restore current r10
    pop r9         ;restore current r9
    pop r8         ;restore current r8
    pop rsi         ;restore current rsix
    pop rdi         ;restore current rdi
    pop rbp         ;restore current rbp
    pop rdx         ;restore current rdx
    pop rcx         ;restore current rcx
    pop rbx         ;restore current rbx
    pop rax         ;restore current rax
%endmacro

%macro hal_interrupt_handler 1
global hal_interrupt_handler_%1
hal_interrupt_handler_%1:
; save top of stack
; NOW STACK:
; +40 SS
; +32 RSP
; +24 RFLAGS
; +16 CS
; +8  RIP
; +0  RBP
push rbp
mov rbp,rsp
PUSHAQ
cld
mov rdi, %1  ; INT VEC #
mov rsi, rbp ; PTR to RIP
add rsi, 8
mov rdx, 0   ; ERROR = 0, in this case
call hal_interrupt_dispatcher
POPAQ
pop rbp
iretq
%endmacro

%macro hal_interrupt_err_handler 1
global hal_interrupt_handler_%1
hal_interrupt_handler_%1:
; save top of stack
; NOW STACK:
; +48 SS
; +40 RSP
; +32 RFLAGS
; +24 CS
; +16 RIP
; +8  ERROR CODE
; +0  RBP
push rbp
mov rbp,rsp

PUSHAQ
cld
mov rdi, %1 ; INT VEC #
mov rsi, rbp ; PTR to RIP
add rsi, 16
mov rdx, qword [rbp + 8] ; ERRPO CODE
call hal_interrupt_dispatcher
POPAQ
pop rbp
add rsp, 8 ; skip the error code
iretq
%endmacro

[SECTION .text]
[BITS 64]

extern hal_interrupt_dispatcher

hal_interrupt_handler 0
hal_interrupt_handler 1
hal_interrupt_handler 2
hal_interrupt_handler 3
hal_interrupt_handler 4
hal_interrupt_handler 5
hal_interrupt_handler 6
hal_interrupt_handler 7
hal_interrupt_err_handler 8
; unused
hal_interrupt_handler 9
hal_interrupt_err_handler 10
hal_interrupt_err_handler 11
hal_interrupt_err_handler 12
hal_interrupt_err_handler 13
hal_interrupt_err_handler 14
; unused
hal_interrupt_handler 15
hal_interrupt_handler 16
hal_interrupt_err_handler 17
hal_interrupt_handler 18
hal_interrupt_handler 19
hal_interrupt_handler 20
; 21-31 unused
hal_interrupt_handler 21
hal_interrupt_handler 22
hal_interrupt_handler 23
hal_interrupt_handler 24
hal_interrupt_handler 25
hal_interrupt_handler 26
hal_interrupt_handler 27
hal_interrupt_handler 28
hal_interrupt_handler 29
hal_interrupt_handler 30
hal_interrupt_handler 31

; user defined
hal_interrupt_handler 32
hal_interrupt_handler 33
hal_interrupt_handler 34
hal_interrupt_handler 35
hal_interrupt_handler 36
hal_interrupt_handler 37
hal_interrupt_handler 38
hal_interrupt_handler 39
hal_interrupt_handler 40
hal_interrupt_handler 41
hal_interrupt_handler 42
hal_interrupt_handler 43
hal_interrupt_handler 44
hal_interrupt_handler 45
hal_interrupt_handler 46
hal_interrupt_handler 47
hal_interrupt_handler 48
hal_interrupt_handler 49
hal_interrupt_handler 50
hal_interrupt_handler 51
hal_interrupt_handler 52
hal_interrupt_handler 53
hal_interrupt_handler 54
hal_interrupt_handler 55
hal_interrupt_handler 56
hal_interrupt_handler 57
hal_interrupt_handler 58
hal_interrupt_handler 59
hal_interrupt_handler 60
hal_interrupt_handler 61
hal_interrupt_handler 62
hal_interrupt_handler 63
hal_interrupt_handler 64
hal_interrupt_handler 65
hal_interrupt_handler 66
hal_interrupt_handler 67
hal_interrupt_handler 68
hal_interrupt_handler 69
hal_interrupt_handler 70
hal_interrupt_handler 71
hal_interrupt_handler 72
hal_interrupt_handler 73
hal_interrupt_handler 74
hal_interrupt_handler 75
hal_interrupt_handler 76
hal_interrupt_handler 77
hal_interrupt_handler 78
hal_interrupt_handler 79
hal_interrupt_handler 80
hal_interrupt_handler 81
hal_interrupt_handler 82
hal_interrupt_handler 83
hal_interrupt_handler 84
hal_interrupt_handler 85
hal_interrupt_handler 86
hal_interrupt_handler 87
hal_interrupt_handler 88
hal_interrupt_handler 89
hal_interrupt_handler 90
hal_interrupt_handler 91
hal_interrupt_handler 92
hal_interrupt_handler 93
hal_interrupt_handler 94
hal_interrupt_handler 95
hal_interrupt_handler 96
hal_interrupt_handler 97
hal_interrupt_handler 98
hal_interrupt_handler 99
hal_interrupt_handler 100
hal_interrupt_handler 101
hal_interrupt_handler 102
hal_interrupt_handler 103
hal_interrupt_handler 104
hal_interrupt_handler 105
hal_interrupt_handler 106
hal_interrupt_handler 107
hal_interrupt_handler 108
hal_interrupt_handler 109
hal_interrupt_handler 110
hal_interrupt_handler 111
hal_interrupt_handler 112
hal_interrupt_handler 113
hal_interrupt_handler 114
hal_interrupt_handler 115
hal_interrupt_handler 116
hal_interrupt_handler 117
hal_interrupt_handler 118
hal_interrupt_handler 119
hal_interrupt_handler 120
hal_interrupt_handler 121
hal_interrupt_handler 122
hal_interrupt_handler 123
hal_interrupt_handler 124
hal_interrupt_handler 125
hal_interrupt_handler 126
hal_interrupt_handler 127
hal_interrupt_handler 128
hal_interrupt_handler 129
hal_interrupt_handler 130
hal_interrupt_handler 131
hal_interrupt_handler 132
hal_interrupt_handler 133
hal_interrupt_handler 134
hal_interrupt_handler 135
hal_interrupt_handler 136
hal_interrupt_handler 137
hal_interrupt_handler 138
hal_interrupt_handler 139
hal_interrupt_handler 140
hal_interrupt_handler 141
hal_interrupt_handler 142
hal_interrupt_handler 143
hal_interrupt_handler 144
hal_interrupt_handler 145
hal_interrupt_handler 146
hal_interrupt_handler 147
hal_interrupt_handler 148
hal_interrupt_handler 149
hal_interrupt_handler 150
hal_interrupt_handler 151
hal_interrupt_handler 152
hal_interrupt_handler 153
hal_interrupt_handler 154
hal_interrupt_handler 155
hal_interrupt_handler 156
hal_interrupt_handler 157
hal_interrupt_handler 158
hal_interrupt_handler 159
hal_interrupt_handler 160
hal_interrupt_handler 161
hal_interrupt_handler 162
hal_interrupt_handler 163
hal_interrupt_handler 164
hal_interrupt_handler 165
hal_interrupt_handler 166
hal_interrupt_handler 167
hal_interrupt_handler 168
hal_interrupt_handler 169
hal_interrupt_handler 170
hal_interrupt_handler 171
hal_interrupt_handler 172
hal_interrupt_handler 173
hal_interrupt_handler 174
hal_interrupt_handler 175
hal_interrupt_handler 176
hal_interrupt_handler 177
hal_interrupt_handler 178
hal_interrupt_handler 179
hal_interrupt_handler 180
hal_interrupt_handler 181
hal_interrupt_handler 182
hal_interrupt_handler 183
hal_interrupt_handler 184
hal_interrupt_handler 185
hal_interrupt_handler 186
hal_interrupt_handler 187
hal_interrupt_handler 188
hal_interrupt_handler 189
hal_interrupt_handler 190
hal_interrupt_handler 191
hal_interrupt_handler 192
hal_interrupt_handler 193
hal_interrupt_handler 194
hal_interrupt_handler 195
hal_interrupt_handler 196
hal_interrupt_handler 197
hal_interrupt_handler 198
hal_interrupt_handler 199
hal_interrupt_handler 200
hal_interrupt_handler 201
hal_interrupt_handler 202
hal_interrupt_handler 203
hal_interrupt_handler 204
hal_interrupt_handler 205
hal_interrupt_handler 206
hal_interrupt_handler 207
hal_interrupt_handler 208
hal_interrupt_handler 209
hal_interrupt_handler 210
hal_interrupt_handler 211
hal_interrupt_handler 212
hal_interrupt_handler 213
hal_interrupt_handler 214
hal_interrupt_handler 215
hal_interrupt_handler 216
hal_interrupt_handler 217
hal_interrupt_handler 218
hal_interrupt_handler 219
hal_interrupt_handler 220
hal_interrupt_handler 221
hal_interrupt_handler 222
hal_interrupt_handler 223
hal_interrupt_handler 224
hal_interrupt_handler 225
hal_interrupt_handler 226
hal_interrupt_handler 227
hal_interrupt_handler 228
hal_interrupt_handler 229
hal_interrupt_handler 230
hal_interrupt_handler 231
hal_interrupt_handler 232
hal_interrupt_handler 233
hal_interrupt_handler 234
hal_interrupt_handler 235
hal_interrupt_handler 236
hal_interrupt_handler 237
hal_interrupt_handler 238
hal_interrupt_handler 239
hal_interrupt_handler 240
hal_interrupt_handler 241
hal_interrupt_handler 242
hal_interrupt_handler 243
hal_interrupt_handler 244
hal_interrupt_handler 245
hal_interrupt_handler 246
hal_interrupt_handler 247
hal_interrupt_handler 248
hal_interrupt_handler 249
hal_interrupt_handler 250
hal_interrupt_handler 251
hal_interrupt_handler 252
hal_interrupt_handler 253
hal_interrupt_handler 254
hal_interrupt_handler 255