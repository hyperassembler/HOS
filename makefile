ASM = nasm

CC = gcc

LD = ld

#Recursive Wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
rdircard=$(sort $(dir $(call rwildcard,$1,*)))

#x86 vars

C_SRC_PATH_32 := x86/src/c

ASM_SRC_PATH_32 := x86/src/asm

C_FLAGS_32 := -m32 -std=c11 -g -c -fno-stack-protector -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -masm=intel -Wall -Wextra

ASM_FLAGS_32 := -f elf32 -I $(ASM_SRC_PATH_32)/

LD_FLAGS_32 := -melf_i386

LD_SCRIPT_32 := build/link32.ld

#x64 vars

C_SRC_PATH_64 := x64/src/c

ASM_SRC_PATH_64 := x64/src/asm

C_FLAGS_64 := -m64 -std=c11 -g -c -fno-stack-protector -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -masm=intel -Wall -Wextra

ASM_FLAGS_64 := -f elf64 -I $(ASM_SRC_PATH_64)/

LD_FLAGS_64 := -melf_x86_64

LD_SCRIPT_64 := build/link64.ld

GRUB_CFG := build/grub.cfg

OUTPUT_DIR := out

ALL_OUTPUT_DIRS := $(addprefix $(OUTPUT_DIR)/,$(call rdircard,*))


#C source code

C_FILES_32 := $(call rwildcard, $(C_SRC_PATH_32), *.c)

C_FILES_64 := $(call rwildcard, $(C_SRC_PATH_64), *.c)



#ASM source code

ASM_FILES_32 := $(call rwildcard, $(ASM_SRC_PATH_32), *.asm)

ASM_FILES_64 := $(call rwildcard, $(ASM_SRC_PATH_64), *.asm)


#bin files

KERNEL_BIN_32 := kernel32.bin

KERNEL_BIN_64 := kernel64.bin


#Object files

C_OBJ_FILES_32 := $(C_FILES_32:.c=.o32)

C_OBJ_FILES_64 := $(C_FILES_64:.c=.o64)

ASM_OBJ_FILES_32 := $(ASM_FILES_32:.asm=.oa32)

ASM_OBJ_FILES_64 := $(ASM_FILES_64:.asm=.oa64)

ALL_OBJ_FILES_32 := $(addprefix $(OUTPUT_DIR)/,$(C_OBJ_FILES_32)) $(addprefix $(OUTPUT_DIR)/,$(ASM_OBJ_FILES_32))

ALL_OBJ_FILES_64 := $(addprefix $(OUTPUT_DIR)/,$(C_OBJ_FILES_64)) $(addprefix $(OUTPUT_DIR)/,$(ASM_OBJ_FILES_64))


#Commands

all: init compile link buildiso clean
# buildsymbol

print_source:
	$(info ${ALL_OUTPUT_DIRS})

init:
	mkdir -p $(ALL_OUTPUT_DIRS)

compile: $(C_OBJ_FILES_32) $(ASM_OBJ_FILES_32) $(C_OBJ_FILES_64) $(ASM_OBJ_FILES_64)


link: $(KERNEL_BIN_32) $(KERNEL_BIN_64)

buildsymbol:
	objcopy --only-keep-debug $(OUTPUT_DIR)/$(KERNEL_BIN_32) $(OUTPUT_DIR)/$(KERNEL_BIN_32).debug
	objcopy --only-keep-debug $(OUTPUT_DIR)/$(KERNEL_BIN_64) $(OUTPUT_DIR)/$(KERNEL_BIN_64).debug
	objcopy --strip-debug $(OUTPUT_DIR)/$(KERNEL_BIN_32)
	objcopy --strip-debug $(OUTPUT_DIR)/$(KERNEL_BIN_64)

buildiso:
	mkdir -p $(OUTPUT_DIR)/temp_iso/HOS
	mkdir -p $(OUTPUT_DIR)/temp_iso/boot
	mkdir -p $(OUTPUT_DIR)/temp_iso/boot/grub
	mv $(OUTPUT_DIR)/$(KERNEL_BIN_64) $(OUTPUT_DIR)/temp_iso/HOS/kernel64
	mv $(OUTPUT_DIR)/$(KERNEL_BIN_32) $(OUTPUT_DIR)/temp_iso/HOS/kernel32
	cp $(GRUB_CFG) $(OUTPUT_DIR)/temp_iso/boot/grub/
	grub-mkrescue -o HOS.iso $(OUTPUT_DIR)/temp_iso
	rm -rf $(OUTPUT_DIR)/temp_iso

clean:
	rm -rf $(OUTPUT_DIR)

%.o32: %.c
	$(CC) $(C_FLAGS_32) -o $(OUTPUT_DIR)/$@ $^

%.o64: %.c
	$(CC) $(C_FLAGS_64) -o $(OUTPUT_DIR)/$@ $^

%.oa32: %.asm
	$(ASM) $(ASM_FLAGS_32) -o $(OUTPUT_DIR)/$@ $^

%.oa64: %.asm
	$(ASM) $(ASM_FLAGS_64) -o $(OUTPUT_DIR)/$@ $^

$(KERNEL_BIN_32): $(ALL_OBJ_FILES_32)
	$(LD) $(LD_FLAGS_32) -T $(LD_SCRIPT_32) -o $(OUTPUT_DIR)/$(KERNEL_BIN_32) $(ALL_OBJ_FILES_32)

$(KERNEL_BIN_64): $(ALL_OBJ_FILES_64)
	$(LD) $(LD_FLAGS_64) -T $(LD_SCRIPT_64) -o $(OUTPUT_DIR)/$(KERNEL_BIN_64) $(ALL_OBJ_FILES_64)

