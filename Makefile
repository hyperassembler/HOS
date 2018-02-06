ASM := nasm
CC := /opt/x86_64-elf-gcc
LD := /opt/x86_64-elf-gcc
DUMP := /opt/x86_64-elf-objdump

LD_SCRIPT := linker.ld

GRUB_CFG := grub.cfg

INCLUDE_DIR := include

MK := mk

C_WARNINGS := -Wall \
			  -Werror \
			  -Wextra \
			  -Wpedantic \
			  -Winit-self \
			  -Wunused-parameter \
			  -Wuninitialized \
			  -Wfloat-equal \
			  -Wshadow \
			  -Wcast-qual \
			  -Wcast-align \
			  -Wstrict-prototypes \
			  -Wpointer-arith \
			  -Wno-comment

C_FLAGS := -std=c11 \
		   -g \
		   -c \
		   -O2 \
		   -mcmodel=kernel \
		   -fno-exceptions \
		   -ffreestanding \
		   -mno-red-zone \
		   -mno-mmx \
		   -mno-sse \
		   -mno-sse2 \
		   -masm=intel \
		   $(C_WARNINGS) \
		   $(addprefix -I, $(INCLUDE_DIR))

ASM_FLAGS := -w+all \
			 -f elf64 \
			 -F dwarf \
			 -g \
			 $(addprefix -I, $(ASM_HEADER_DIRS))

LD_FLAGS := -lgcc \
            -nodefaultlibs \
			-nostartfiles \
			-nostdlib \
			-Wl,-n \
			-Wl,--build-id=none

COMP :=
LINK :=
COMPAS :=
PACK := 

include Rules.mk