include(${MK}/common.cmake)

#
# Standard rules for kernel
#
# Process AS_SRC and CC_SRC and add them to "OBJS"
#

FOREACH(f IN LISTS AS_SRC)
    PP_GEN(${CMAKE_CURRENT_SOURCE_DIR}/${f} AS_TMPSRC)
endforeach(f)

FOREACH(f IN LISTS AS_TMPSRC)
    AS_GEN(${f} M_OBJS)
endforeach(f)

FOREACH(f IN LISTS CC_SRC)
    CC_GEN(${CMAKE_CURRENT_SOURCE_DIR}/${f} M_OBJS)
endforeach(f)

set(SUBTARGET ${CMAKE_CURRENT_BINARY_DIR}/${SUBMODULE}.o)
# Rules for generating the target
add_custom_command(
        OUTPUT ${SUBTARGET}
        DEPENDS ${M_OBJS}
        COMMAND ${CC} ${LD_FLAGS} -r -o ${SUBTARGET} ${M_OBJS})

add_custom_target(${SUBMODULE}
        DEPENDS ${SUBTARGET})
 
set(OBJS_${SUBMODULE} ${SUBTARGET} PARENT_SCOPE)
set(TARGET_${SUBMODULE} ${SUBMODULE} PARENT_SCOPE)

# hack for clion not parsing custom targets
set(CUR_CC_SRC "")

FOREACH(f IN LISTS CC_SRC)
    set(CUR_CC_SRC ${CUR_CC_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
endforeach(f)

set(G_CC_SRC ${G_CC_SRC} ${CUR_CC_SRC} PARENT_SCOPE)