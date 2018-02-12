CROSS_DIR = ~/opt/cross/bin
AS = nasm
CC = $(CROSS_DIR)/x86_64-elf-gcc
LD = $(CROSS_DIR)/x86_64-elf-gcc
DAS = $(CROSS_DIR)/x86_64-elf-objdump

INCLUDE_DIR = include
MK = mk

LD_SCRIPT = $(MK)/linker.ld
GRUB_CFG = $(MK)/grub.cfg

C_WARNINGS = -Wall \
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

C_FLAGS = -std=c11 \
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

AS_FLAGS = -w+all \
			 -f elf64 \
			 -F dwarf \
			 -g \
			 $(addprefix -I, $(INCLUDE_DIR)/)

LD_FLAGS =  -lgcc \
            -nodefaultlibs \
			-nostartfiles \
			-nostdlib \
			-Wl,-n \
			-Wl,--build-id=none

DUMP_FLAGS = -M intel \
			 -D

PREP_FLAGS = -E \
			 -x c \
			 -P \
			 -traditional-cpp \
			 $(C_FLAGS)

GDEP_FLAGS = $(PREP_FLAGS) \
			 -MMD \
			 -MT $@
			 

COMP = $(CC) $(C_FLAGS) $< -o $@
COMPAS = $(AS) $(AS_FLAGS) $< -o $@
LINK = $(LD) $(LD_FLAGS) $^ -o $@
DUMP = $(DAS) $(DUMP_FLAGS) $< > $@
PREP = $(CC) $(PREP_FLAGS) $< > $@
GDEP = $(CC) $(GDEP_FLAGS) -MF $*.d $< > /dev/null

include Rules.top