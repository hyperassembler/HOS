# toolchain file for amd64 target
find_program(AS nasm)
find_program(CC clang)
find_program(LD ld.lld)
find_program(DMP objdump)
find_program(GRUB grub2-mkrescue)


set(CC_FLAGS_${ARCH}
		--target=x86_64-unknown-none-elf
        -mcmodel=kernel
        -mno-red-zone
        -mno-mmx
        -mno-sse
        -mno-sse2
        -mno-sse3
        -mno-3dnow)

set(AS_FLAGS_${ARCH}
        -w+all
        -w+error
        -Fdwarf
        -g
        -felf64)

set(DMP_FLAGS_${ARCH}
        -M intel
        -d
        -t)
