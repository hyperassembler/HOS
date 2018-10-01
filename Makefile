AS := nasm
CC := clang
LD := clang
DAS := llvm-objdump

INC_COMMON := inc
MK := mk
OUT := out

C_IGNORED_WARNINGS = -Wno-cast-align \
					 -Wno-padded

C_FLAGS =   -xc\
            -g \
            -c \
            -O2 \
			-std=c11 \
			-Weverything \
			-Werror \
			$(C_IGNORED_WARNINGS) \
			-ffreestanding \
			-fno-builtin \
			-nostdlib \
			-fno-pic \
			-mcmodel=kernel \
			-fno-stack-protector \
			-mno-red-zone \
			-mno-mmx \
			-mno-sse \
			-mno-sse2 \
			-mno-sse3 \
			-mno-3dnow \
			-target x86_64-pc-none-elf \
			-I$(INC_COMMON)

AS_FLAGS =  -w+all \
			-w+error \
			-f elf64 \
			-F dwarf \
			-g \
			-I$(INC_COMMON) \
			$(INC_$(d))

LD_FLAGS =  -fuse-ld=lld \
			-nostdlib \
			-Wl,-T,$(LD_SCRIPT) \
			-Wl,--fatal-warnings

DUMP_FLAGS = -x86-asm-syntax=intel \
			 -disassemble \
			 -r \
			 -t \
			 -triple=x86_64-elf

PREP_FLAGS = -E \
			 -xc\
			 -P \
			 $(C_FLAGS)

GDEP_FLAGS = $(PREP_FLAGS) \
			 -MMD \
			 -MT $@

MKDIR = mkdir -p $(dir $@)
COMP = $(CC) $(C_FLAGS) -o $@ $<
COMPAS = $(AS) $(AS_FLAGS) -o $@ $<
LINK = $(LD) $(LD_FLAGS) -o $@ $^
DUMP = $(DAS) $(DUMP_FLAGS) $< > $@
PREP = $(CC) $(PREP_FLAGS) $< > $@
GDEP = $(CC) $(GDEP_FLAGS) -MF $(addsuffix .d, $@) $< > /dev/null

include Rules.top
