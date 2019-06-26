set(SUBMODULE kern)
set(CC_SRC
        assert.c
        atree.c
        clib.c
        main.c
        panic.c
        print.c
        spin_lock.c
        list.c )

include(${MK}/kern.cmake)

