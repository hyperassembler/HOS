.text
.align 4
.global ts_arch_startNextIteration_ASM
.global ts_arch_enter_init_cleanup_ASM
.global ts_arch_resume_test_ASM
.global ts_arch_resume_kernel_ASM
.global ts_arch_runIdleThread_ASM

// Possibly needs to save off program state and processor status for
// context switches
        
// Inputs: RDI = Pointer to the register save space
.macro TS_ARCH_SAVE_REGS_ASM
        mov %rax, 0x00(%rdi) 
        mov %rbx, 0x08(%rdi) 
        mov %rcx, 0x10(%rdi) 
        mov %rdx, 0x18(%rdi) 
        mov %rsp, 0x20(%rdi) 
        mov %rbp, 0x28(%rdi) 
        mov %rsi, 0x30(%rdi) 
        mov %rdi, 0x38(%rdi) 
        mov  %r8, 0x40(%rdi) 
        mov  %r9, 0x48(%rdi) 
        mov %r10, 0x50(%rdi) 
        mov %r11, 0x58(%rdi) 
        mov %r12, 0x60(%rdi) 
        mov %r13, 0x68(%rdi) 
        mov %r14, 0x70(%rdi) 
        mov %r15, 0x78(%rdi)
.endm

// Inputs: RDI = Pointer to the register save space
.macro TS_ARCH_RESTORE_REGS_ASM
        mov 0x00(%rdi), %rax 
        mov 0x08(%rdi), %rbx 
        mov 0x10(%rdi), %rcx 
        mov 0x18(%rdi), %rdx 
        mov 0x20(%rdi), %rsp 
        mov 0x28(%rdi), %rbp 
        mov 0x30(%rdi), %rsi 
        mov 0x40(%rdi),  %r8 
        mov 0x48(%rdi),  %r9 
        mov 0x50(%rdi), %r10 
        mov 0x58(%rdi), %r11 
        mov 0x60(%rdi), %r12 
        mov 0x68(%rdi), %r13 
        mov 0x70(%rdi), %r14 
        mov 0x78(%rdi), %r15
        mov 0x38(%rdi), %rdi
.endm


// Inputs: RDI = Pointer to the register save space of the current test/kernel
//         RSI = Pointer to the register save space of the test/kernel to restore
ts_arch_resume_test_ASM:
        TS_ARCH_SAVE_REGS_ASM
        mov %rsi, %rdi
        TS_ARCH_RESTORE_REGS_ASM        
        mov $0, %rax
        ret

// Inputs: RDI = Pointer to the register save space of the current test
//         RSI = Pointer to the register save space of the kernel to restore
ts_arch_resume_kernel_ASM:
        TS_ARCH_SAVE_REGS_ASM
        mov %rsi, %rdi
        TS_ARCH_RESTORE_REGS_ASM
        mov $0, %rax
        ret

// Inputs: RDI = Pointer to the register save space of the current test
//         RSI = Pointer to the ts_poll function
ts_arch_runIdleThread_ASM:

        //Return -- Not yet implemented in X86
        ret

// Inputs: RDI = Pointer to the register save space of the kernel
//         RSI = Pointer to the stack space of the test
//         RDX = Pointer to the runTest_* function
//         RCX = Iteration argument for runTest_*
ts_arch_enter_init_cleanup_ASM:
        // Save all the registers
        TS_ARCH_SAVE_REGS_ASM

        // Temporary save the kernel stack we'll return to
        mov %rsp, %rax

        // Change the stack
        mov %rsi, %rsp

        // Save the kernel register storage space pointer so we can restore registers on the return
        push %rdi

        // Load the iteration argument
        mov %rcx, %rdi

        // Call runTest_*
        mov %rcx, %rdi
        call *%rdx

        // Pop the kernelRegs pointer
        pop %rdi

        // Restore all the registers
        TS_ARCH_RESTORE_REGS_ASM

        // Return 1 to indicate that this is a return from runTest_ instead of a resume kernel
        mov $1, %rax
        ret

// Inputs: RDI = Pointer to the register save space of current test
//         RSI = Pointer to the stack space of the next test
//         RDX = Pointer to the runTest_* function
//         RCX = Iteration argument for runTest_*
ts_arch_startNextIteration_ASM:

        //Not yet implemented
        ret