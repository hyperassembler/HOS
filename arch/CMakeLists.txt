set(SUBMODULE arch)
set(CC_SRC
        cpu.c
        main.c
        intr.c
        print.c
        brute.c
        pmap.c
        paging.c)

set(AS_SRC
        boot.asm
        mb_hdr.asm
        cpu.asm
        intr.asm)

include(${MK}/kern.cmake)