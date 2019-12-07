set(SUBMODULE arch)
set(CC_SRC
        cpu.c
        main.c
        intr.c
        mem.c
        print.c
        brute.c)

set(AS_SRC
        boot.asm
        mb_hdr.asm
        cpu.asm
        intr.asm)

include(${MK}/kern.cmake)