
AS := nasm
CC := clang
LD := lld
DAS := llvm-objdump-6.0

ifneq '$(AS_ENV)' ''
AS := $(AS_ENV)
endif
$(info Using AS=$(AS))

ifneq '$(CC_ENV)' ''
CC := $(CC_ENV)
endif
$(info Using CC=$(CC))

ifneq '$(LD_ENV)' ''
LD := $(LD_ENV)
endif
$(info Using LD=$(LD))

ifneq '$(DAS_ENV)' ''
DAS := $(DAS_ENV)
endif
$(info Using DAS=$(DAS))

INC_COMMON := inc
MK := mk
OUT := out

C_FLAGS_ARCH_X86_64 := -mcmodel=kernel \
                  -target x86_64-pc-none-elf \
                  -mno-red-zone \
                  -mno-mmx \
                  -mno-sse \
                  -mno-sse2 \
                  -mno-sse3 \
                  -mno-3dnow

# generic freestanding cflags used for target
# each submodule can append to this flag
C_FLAGS =   -x c \
			-g \
            -c \
            -O2 \
			-std=c17 \
			-Wall \
			-Wextra \
			-Wpedantic \
			-Werror \
			-ffreestanding \
			-fno-pic \
			-fno-stack-protector \
			$(C_FLAGS_ARCH_X86_64) \
			-I$(INC_COMMON) \
			$(C_FLAGS_$(MOD))

# generic asm flags used for target
# each submodule can append to this flag
AS_FLAGS =  -w+all \
			-w+error \
			-f elf64 \
			-F dwarf \
			-g \
			-I$(INC_COMMON) \
			$(AS_FLAGS_$(MOD))

# generic pre-processing flags used for target
PREP_FLAGS = -E \
			 -xc\
			 -P \
			 -I$(INC_COMMON) \
             $(C_FLAGS_$(MOD))

# generic generate dependency flags used for target
# each submodule can append to this flag
GDEP_FLAGS = $(PREP_FLAGS) \
			 -MMD \
			 -MT $@

MKDIR = mkdir -p $(dir $@)
COMP = $(CC) $(C_FLAGS) -o $@ $<
COMPAS = $(AS) $(AS_FLAGS) -o $@ $<
PREP = $(CC) $(PREP_FLAGS) $< > $@
GDEP = $(CC) $(GDEP_FLAGS) -MF $(addsuffix .d, $@) $< > /dev/null

include Rules.top
