set(SUBMODULE kern)
set(CC_SRC
        avl_tree.c
        libkern.c
        kmain.c
        print.c
        spin_lock.c
        )

include(${MK}/kern.cmake)

