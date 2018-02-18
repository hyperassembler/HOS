CROSS_PATH = ~/opt/cross/bin
AS = nasm
CC = $(CROSS_PATH)/x86_64-elf-gcc
LD = $(CROSS_PATH)/x86_64-elf-ld
DAS = $(CROSS_PATH)/x86_64-elf-objdump

INCLUDE_DIR = include
MK = mk
OUT = out

C_WARNINGS =	-Wall \
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

C_FLAGS =   -std=c11 \
			-g \
			-c \
			-O2 \
			-mcmodel=kernel \
			-fno-stack-protector \
			-ffreestanding \
			-mno-red-zone \
			$(C_WARNINGS) \
			$(addprefix -I, $(INCLUDE_DIR))

AS_FLAGS =  -w+all \
			-w+error \
			-f elf64 \
			-F dwarf \
			-g \
			$(addprefix -I, $(INCLUDE_DIR)/)

LD_FLAGS =  -nostdlib \
			--fatal-warnings

DUMP_FLAGS = -M intel \
			 -d

PREP_FLAGS = -E \
			 -x c \
			 -P \
			 $(C_FLAGS)

GDEP_FLAGS = $(PREP_FLAGS) \
			 -MMD \
			 -MT $@

MKDIR = mkdir -p $(dir $@)
COMP = $(CC) $(C_FLAGS) -o $@ $<
COMPAS = $(AS) $(AS_FLAGS) -o $@ $<
LINK = $(LD) $(LD_FLAGS) -o $@ $^ $(shell $(CC) $(C_FLAGS) -print-libgcc-file-name)
DUMP = $(DAS) $(DUMP_FLAGS) $< > $@
PREP = $(CC) $(PREP_FLAGS) $< > $@
GDEP = $(CC) $(GDEP_FLAGS) -MF $(addsuffix .d, $@) $< > /dev/null

include Rules.top