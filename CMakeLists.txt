cmake_minimum_required(VERSION 3.10)

# disable in-source build
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(bond)

set(proj bond)
set(INC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(MK ${CMAKE_CURRENT_SOURCE_DIR}/mk)
set(arch amd64)

include(${MK}/${arch}.cmake)

# sanity check
set(TOOLCHAINS
        AS
        CC
        GRUB
        LD
        DMP)

FOREACH (f IN LISTS TOOLCHAINS)
    if (NOT ${f})
        message(FATAL_ERROR "Toolchain ${f} cannot be found.")
    endif ()
endforeach (f)

set(CC_WARN_IGNORE
        -Wno-gnu-statement-expression # this means we can assign the return val of the last statment in a block, useful in MAX, MIN, etc.
        -Wno-gnu-zero-variadic-macro-arguments # this enables us to use __VA_ARGS__ in macros
        )

set(CC_FLAGS
        -std=c17
        -c
        -g
        -O0
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        ${CC_WARN_IGNORE}
        -ffreestanding
        -I${INC}
        -fno-pic
        -fno-stack-protector
        ${CC_FLAGS_${ARCH}})

set(AS_FLAGS
        ${AS_FLAGS_${ARCH}})

set(LD_FLAGS
        -fuse-ld=${LD}
        -nostdlib
        -Wl,--fatal-warnings
        -Wl,--build-id=none
        ${LD_FLAGS_${ARCH}})

set(PP_FLAGS
        -E
        -xc
        -P
        -I${INC}
        ${PP_FLAGS_${ARCH}})

set(DMP_FLAGS
        ${DMP_FLAGS_${ARCH}})

set(SUBMODULES
        common
        ke
        arch
        mm
        scripts
        test)

# process submodules
FOREACH (f IN LISTS SUBMODULES)
    add_subdirectory(${f})
endforeach (f)

# process dependencies
FOREACH (f IN LISTS SUBMODULES)
    set(OBJS ${OBJS} ${OBJS_${f}})
    set(TARGETS ${TARGETS} ${TARGET_${f}})
endforeach (f)

# set target names


# Rules for generating the image
set(TARGET ${proj}_elf)
set(OBJ_${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${proj}.elf)
add_custom_command(
        OUTPUT ${OBJ_${TARGET}}
        DEPENDS ${TARGETS} ${OBJS}
        COMMAND ${CC} ${LD_FLAGS} -Wl,-T, ${SCRIPT_LD} -o ${OBJ_${TARGET}} ${OBJS})

add_custom_target(${TARGET} ALL
        DEPENDS ${OBJ_${TARGET}})


# Rules for generating the dump
set(TARGET ${proj}_dmp)
set(OBJ_${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${proj}.dmp)

add_custom_command(
        OUTPUT ${OBJ_${TARGET}}
        DEPENDS ${proj}.elf
        COMMAND ${DMP} ${DMP_FLAGS} ${OBJ_${proj}_elf} > ${OBJ_${TARGET}}
)
add_custom_target(${proj}_dmp ALL
        DEPENDS ${OBJ_${TARGET}})


# Rules for generating the iso
set(TARGET ${proj}_iso)
set(OBJ_${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${proj}.iso)

add_custom_command(
        OUTPUT ${OBJ_${TARGET}}
        DEPENDS ${proj}.elf
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tmp_iso
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tmp_iso/boot
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tmp_iso/boot/grub
        COMMAND ${CMAKE_COMMAND} -E copy ${OBJ_${proj}_elf} ${CMAKE_CURRENT_BINARY_DIR}/tmp_iso/
        COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPT_GRUB} ${CMAKE_CURRENT_BINARY_DIR}/tmp_iso/boot/grub/
        COMMAND ${GRUB} -d /usr/lib/grub/i386-pc -o ${OBJ_${TARGET}} ${CMAKE_CURRENT_BINARY_DIR}/tmp_iso
)

add_custom_target(${proj}_iso ALL
        DEPENDS ${OBJ_${TARGET}})

# hack for clion not parsing custom targets
if ($ENV{CLION_IDE})
    SET(CMAKE_C_COMPILER ${CC})
    include_directories(${INC})
    add_executable(kernel ${G_CC_SRC})
    add_definitions(-DKOPT_DBG)
    target_compile_options(kernel PRIVATE ${CC_FLAGS})
endif ()


