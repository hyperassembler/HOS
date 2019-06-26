set(SUBMODULE scripts)

set(GRUB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg)

set(LD_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/linker.lds)

include (${MK}/common.cmake)

PP_GEN(${GRUB_SRC} GRUB_CFG)
PP_GEN(${LD_SRC} LD_LDS)

add_custom_target(${SUBMODULE}
        DEPENDS ${GRUB_CFG}
        DEPENDS ${LD_LDS})

set(TARGET_${SUBMODULE} ${SUBMODULE} PARENT_SCOPE)
set(SCRIPT_LD ${LD_LDS} PARENT_SCOPE)
set(SCRIPT_GRUB ${GRUB_CFG} PARENT_SCOPE)