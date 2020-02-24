set(SUBMODULE ke)
set(CC_SRC
        libkern.c
        kmain.c
        print.c
        spin_lock.c
        )

include(${MK}/kern.cmake)

