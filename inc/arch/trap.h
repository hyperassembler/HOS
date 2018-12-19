#pragma once

/**
 * Trap frame
 */
struct trap_frame
{
    uint64 gs;
    uint64 fs;
    uint64 es;
    uint64 ds;
    uint64 r15;
    uint64 r14;
    uint64 r13;
    uint64 r12;
    uint64 r11;
    uint64 r10;
    uint64 r9;
    uint64 r8;
    uint64 rsi;
    uint64 rdi;
    uint64 rbp;
    uint64 rdx;
    uint64 rcx;
    uint64 rbx;
    uint64 rax;
    uint64 vec;
    uint64 error_code;
    uint64 rip;
    uint64 cs;
    uint64 rflags;
    uint64 rsp;
    uint64 ss;
};